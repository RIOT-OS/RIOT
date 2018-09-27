#!/usr/bin/env python

import yaml
import argparse

DEFINE_PREVIOUS = '#ifndef {0}\n'
DEFINE_STRING = '#define {0} {1}\n'
DEFINE_NEXT = '#endif\n\n'
DEFAULT_OUTPUT_FILE = 'config.h'

class ConfigGenerator():
    """ A configuration file generator """

    def __init__(self, configuration, output_file = DEFAULT_OUTPUT_FILE):
        """ """
        self.configuration = configuration
        self.module_name = configuration.get('name')
        self.output_file = output_file

    def generate_configuration(self):
        if self.module_name == None:
            print('[ERROR] No module name defined')
            return

        module_config = self.configuration.get('configuration')
        output = ''

        output += self.parse_configuration_group(module_config)
        self.write_output_file(output)

    def parse_configuration_group(self, group_parameters, group_name = []):
        """ Parses a configuration group and returns the generated string. """
        output = ''

        for parameter in group_parameters:
            if parameter.get('group'):
                _group_name = group_name + [parameter['group']['name']]
                output += self.parse_configuration_group( \
                    parameter['group']['parameters'],     \
                    _group_name)
            else:
                output += self.parse_configuration_parameter(parameter, \
                                                             group_name)

        return output

    def parse_configuration_parameter(self, parameter, group_name = []):
        output = ''
        
        name = self.format_parameter_name(parameter['name'], group_name, \
                                          parameter.get('macro'))

        value = self.format_parameter_value(parameter['value'], \
                                            parameter['type'],  \
                                            parameter.get('properties', {}))

        output += DEFINE_PREVIOUS.format(name)
        output += DEFINE_STRING.format(name, value)
        output += DEFINE_NEXT
        return output

    def write_output_file(self, content = ''):
        output = (
        '/* This is an automatically generated configuration file\n'
         '* DO NOT EDIT, the content will be overwritten.\n'
         '*/\n'
         '#ifndef {0}_CONFIG_H\n'
         '#define {0}_CONFIG_H\n\n'
         '{1}'
         '#endif /* {0}_CONFIG_H */\n'
        )
        output_name = self.output_file.split('/')
        output_name = output_name[-1].split('.')[0].replace('-', '_').upper()
        output = output.format(output_name, content)

        try:
            with open(self.output_file, 'w') as out_file:
                out_file.write(output)
                out_file.close()
        except expression as identifier:
            print('[ERROR] Could not generate configuration file')
        else:
            print('[INFO] Configuration file generated in {}' \
                  .format(self.output_file))

    def format_parameter_name(self, name, group_name = [], macro = None):
        output = ['CONFIG']

        if macro:
            output += [macro]
        else:
            output += [self.module_name]
            output += group_name
            output += [name]

        return '_'.join(output).replace('-','_').upper()

    def format_parameter_value(self, value, type, properties):
        if type == 'byte':
            if properties.get('size'):
                # If the amount of bytes is defined pad with zeros
                size = properties['size']
                return '"{0:0{1}X}"'.format(value, size * 2)
            else:
                return hex(value)
        else:
            return value


if __name__ == '__main__':

    parser = argparse.ArgumentParser( \
        description= '''Configuration file generator.''')

    parser.add_argument('configuration_file', \
        help='File containing the configuration to parse')
    parser.add_argument('output_file', \
        help='Path to the file where to output the configuration')

    args = parser.parse_args()

    try:
        with open(args.configuration_file) as f:
            text = f.read()
            f.close()
    except:
        print('[ERROR] Could not open configuration file {}' \
              .format(args.configuration_file))
    else:
        output = ''

        configuration = yaml.load(text)

        generator = ConfigGenerator(configuration, args.output_file)

        generator.generate_configuration()

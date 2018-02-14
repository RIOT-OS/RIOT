SECTIONS
{
  .data :
  {
    _shell_command_list = .; /* begining of the command list */
    KEEP(*(._shell_command_list))
    KEEP(*(._shell_command_list_end))
  } > ram
}

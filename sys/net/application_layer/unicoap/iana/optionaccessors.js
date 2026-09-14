// This JavaScript expression returns a string of C option accessors to be pasted
// into sys/include/net/unicoap/options/generated.h

(() => {
    let OPTIONS = {
        "If-Match": {
            format: "bytes",
            sizeMax: 8,
            repeatable: true,
            contiguousRepresentation: null,
            singular: "value",
            pluralLong: "all if match"
        },
        "Uri-Host": {
            format: "utf8",
            sizeMin: 1,
            sizeMax: 0xff,
            repeatable: false,
            singular: "host"
        },
        "ETag": {
            format: "bytes",
            sizeMin: 1,
            sizeMax: 8,
            repeatable: true,
            contiguousRepresentation: null,
            singular: "etag",
            singularLong: "etag",
            plural: "etags",
            pluralLong: "etags"
        },
        "If-None-Match": {
            format: "bool",
            repeatable: false,
            singular: "value"
        },
        "Observe": {
            format: "uint24",
            repeatable: false,
            singular: "observe",
            section: "Observe"
        },
        "Uri-Port": {
            format: "uint16",
            repeatable: false,
            singular: "port"
        },
        "Location-Path": {
            format: "utf8",
            repeatable: true,
            contiguousRepresentation: "absolute location path",
            separator: "/",
            prefix: "/",
            prefixIfEmpty: true,
            continuous: "path",
            singular: "component",
            singularLong: "location path component",
            plural: "components",
            pluralLong: "location path",
        },
        "Uri-Path": {
            format: "utf8",
            sizeMax: 0xff,
            repeatable: true,
            contiguousRepresentation: "absolute URI path",
            separator: "/",
            prefix: "/",
            prefixIfEmpty: true,
            continuous: "path",
            singular: "component",
            singularLong: "URI path component",
            plural: "components",
            pluralLong: "URI path",
        },
        "Content-Format": {
            format: "uint16",
            formatType: "unicoap_content_format_t",
            repeatable: false,
            singular: "format"
        },
        "Max-Age": {
            format: "uint32",
            repeatable: false,
            singular: "age"
        },
        "Uri-Query": {
            format: "utf8",
            sizeMax: 0xff,
            repeatable: true,
            contiguousRepresentation: "URI query string",
            separator: "&",
            prefix: null,
            prefixIfEmpty: false,
            continuous: "queries",
            singular: "query",
            singularLong: "URI query",
            plural: "queries",
            pluralLong: "URI queries",
            supportsQueryByName: true
        },
        "Accept": {
            format: "uint16",
            formatType: "unicoap_content_format_t",
            repeatable: false,
            singular: "format"
        },
        "Location-Query": {
            format: "utf8",
            sizeMax: 0xff,
            repeatable: true,
            contiguousRepresentation: "location query string",
            separator: "&",
            prefix: null,
            prefixIfEmpty: false,
            continuous: "queries",
            singular: "query",
            singularLong: "location query",
            plural: "queries",
            pluralLong: "location queries",
            supportsQueryByName: true,
        },
        "Proxy-Scheme": {
            format: "utf8",
            repeatable: false,
            singular: "scheme"
        },
        "Proxy-Uri": {
            format: "utf8",
            sizeMin: 1,
            sizeMax: 1034,
            repeatable: false,
            singular: "uri"
        },
        "No-Response": {
            format: "uint8",
            repeatable: false,
            singular: "value"
        },
        "Block1": {
            format: "block",
            formatType: "unicoap_block_option_t",
            repeatable: false,
            section: "Block-wise Transfers",
            singular: "block"
        },
        "Block2": {
            format: "block",
            formatType: "unicoap_block_option_t",
            repeatable: false,
            section: "Block-wise Transfers",
            singular: "block"
        },
        "Size1": {
            format: "uint32",
            repeatable: false,
            section: "Block-wise Transfers",
            singular: "size"
        },
        "Size2": {
            format: "uint32",
            repeatable: false,
            section: "Block-wise Transfers",
            singular: "size"
        },
    }

    for (const [key, value] of Object.entries(OPTIONS)) {
        OPTIONS[key].name = key
    }

    let suffix = (o) => !!o.singularLong && o.repeatable ? o.singularLong.replace(/\s/g, "_").toLowerCase() : o.name.replace(/-/g, "_").toLowerCase()

    let suffixPlural = (o) => !!o.pluralLong ? o.pluralLong.toLowerCase().replace(/\s/g, "_") : suffix(o)

    let suffixMultiple = (o) => o.multiple.toLowerCase().replace(/\s/g, "_")

    let constant = (o) => `UNICOAP_OPTION_${o.name.replace(/-/g, "_").toUpperCase()}`

    let type = (o, underlying) => {
        if (!!o.formatType && !underlying) {
            return o.formatType
        }

        switch (o.format) {
                case "bool": return "bool"
                case "utf8": return "char*"
                case "bytes": return "uint8_t*"
                case "uint8": return "uint8_t"
                case "uint16": return "uint16_t"
                case "uint24": return "uint32_t"
                case "uint32": return "uint32_t"
                default: return "void*"
        }
    }

    let cast = (o, pointer, qualifiers) => {
        if (!!o.formatType) {
            return `(${qualifiers ? qualifiers : ""}${type(o, true)}${pointer ? "*" : ""})`
        } else if (o.format == "utf8") {
            return `(${qualifiers ? qualifiers : ""}uint8_t*${pointer ? "*" : ""})`
        } else {
            return ""
        }
    }

    let getterNonRepeatable = (o) => {
        switch (o.format) {
            case "block":
                return `return unicoap_options_get_block(options, ${constant(o)}, ${o.singular});`
            case "bool":
                return `return unicoap_options_contains(options, ${constant(o)});`
            case "utf8":
                return `return unicoap_options_get(options, ${constant(o)}, ${cast(o, true, "const ")}${o.singular});`
            case "bytes":
                return `return unicoap_options_get(options, ${constant(o)}, ${cast(o, true, "const")}${o.singular});`
            case "uint8":
                return `return unicoap_options_get_uint8(options, ${constant(o)}, ${cast(o, true)}${o.singular});`
            case "uint16":
                return `return unicoap_options_get_uint16(options, ${constant(o)}, ${cast(o, true)}${o.singular});`
            case "uint24":
                return `return unicoap_options_get_uint24(options, ${constant(o)}, ${cast(o, true)}${o.singular});`
            case "uint32":
                return `return unicoap_options_get_uint32(options, ${constant(o)}, ${cast(o, true)}${o.singular});`
        }
    }


    let sizeParamName = (o) => {
        switch (o.format) {
                case "bytes": return "size"
                case "utf8": return "length"
                default: return ""
        }
    }

    let sizeParamDescription = (o, buffer) => {
        switch (o.format) {
            case "bytes": return `Number of bytes in @p ${buffer}`
            case "utf8": return `Number of UTF-8 code units in @p ${buffer}, excluding null-terminator (if present)`
            default: return ""
        }
    }

    let sizeParam = (o) => {
        let name = sizeParamName(o)
        if (name.length > 0) {
            return `, size_t ${name}`
        } else {
            return name
        }
    }

    let op = (o) => o.repeatable ? "add" : "set"

    let _set = (o) => {
        switch (o.format) {
            case "block":
                return `return unicoap_options_set_block(options, ${constant(o)}, ${o.singular});`
            case "bool":
                return `if (value) {
        return unicoap_options_remove(options, ${constant(o)});
    }
    else {
        return unicoap_options_set(options, ${constant(o)}, NULL, 0);
    }`
            case "utf8":
                return `return unicoap_options_${op(o)}(options, ${constant(o)}, ${cast(o)}${o.singular}, ${sizeParamName(o)});`
            case "bytes":
                return `return unicoap_options_${op(o)}(options, ${constant(o)}, ${cast(o)}${o.singular}, ${sizeParamName(o)});`
            case "uint8":
            case "uint16":
            case "uint24":
            case "uint32":
                return `return unicoap_options_${op(o)}_uint(options, ${constant(o)}, ${type(o) == "uint32_t" ? "" : "(uint32_t)"}${o.singular});`
        }
    }


    let removeNonRepeatable = (o) => `return unicoap_options_remove(options, ${constant(o)});`
    let removeRepeatable = (o) => `return unicoap_options_remove_all(options, ${constant(o)});`

    let assertPre = (o) => {
        let size = sizeParamName(o)
        let out = ""
        if (o.sizeMin) {
            out += `${size} ${o.sizeMin == 1 ? "> 0" : `>= ${o.sizeMin}`}`
        }

        if (o.sizeMax) {
            if (out.length > 0) {
                out += " && "
            }

            out += `${size} <= ${o.sizeMax}`
        }

        if (out.length > 0) {
            out = `assert(${out});\n    `
        }
        return out
    }

    let docMember = () => `@memberof unicoap_options_t`
    let docOpts = (option, mutate) =>
        `@param[in${mutate ? ",out" : ""}] options Options to ${mutate ? "write to" : "read from"}`
    let docNum = (option) =>
        `@param number Option number`
    let docParamSetNonRepeatable = (o) =>
        `@param${(o.format == "bytes" || o.format == "utf8") ? "[in]" : ""} ${o.singular} \`${o.name}\` value`
    let docParamSize = (o, buffer) =>
        `@param ${sizeParamName(o)} ${sizeParamDescription(o, buffer)}`
    let docParamCapacity = (o, buffer) =>
        `@param capacity Capacity of @p ${buffer}`
    let docParamGetNonRepeatable = (o) =>
        `@param[out] ${o.singular} Pointer to value variable`
    let docRetSize = (o) =>
        `@returns Size of option value in bytes on success, negative error number otherwise`
    let docRetLength = (o) =>
        `@returns Length of string value in bytes on success, negative error number otherwise`
    let docRetZero = (option) =>
        `@returns Zero on success, negative error number otherwise`
    let docRetNoBufSSet = (option) =>
        `@retval \`-ENOBUFS\` Options buffer lacks sufficient capacity to ${option.repeatable ? "add options" : "set option"}`
    let docRetNoBufSCopy = (option, buffer) =>
        `@retval \`-ENOBUFS\` @p ${buffer} lacks sufficient capacity to copy values`
    let docRetNotFound = (option) =>
        `@retval \`-ENOENT\` Option not found`
    let docRetCorrupted = (option) =>
        `@retval \`-EBADOPT\` Option corrupted`
    let docPreMin = (o, buffer) => o.sizeMin ? `\n * @pre ${buffer} must be at least of size ${o.sizeMin} bytes` : ""
    let docPreMax = (o, buffer) => o.sizeMax ? `\n * @pre ${buffer} size must not exceed ${o.sizeMax} bytes` : ""
    let docPre = (o, buffer) => "\n *" + docPreMin(o, buffer) + docPreMax(o, buffer)


    let section = (name, symbols) => `
/* MARK: - ${name} */
/**
 * @name ${name}
 * @{
 */
${symbols}
/** @} */ /* ${name} */`

    let getterBool = (o) => `
/**
 * @brief Determines whether the \`${o.name}\` option is present
 * ${docMember()}
 *
 * ${docOpts(o, false)}
 *
 * ${docRetSize(o)}
 * ${docRetNotFound(o)}
 * ${docRetCorrupted(o)}
 */
static inline ${type(o)} unicoap_options_get_${suffix(o)}(const unicoap_options_t* options)
{
    ${getterNonRepeatable(o)}
}`

    let getterUTF8 = (o, first) => `
/**
 * @brief Retrieves the${first ? " first" : ""} \`${o.name}\` option, if present
 * ${docMember()}
 *
 * ${docOpts(o, false)}
 * ${docParamGetNonRepeatable(o)} as buffer of UTF-8 code units
 *
 * ${docRetSize(o)}
 * ${docRetNotFound(o)}
 * ${docRetCorrupted(o)}
 */
static inline ssize_t unicoap_options_get${first ? "_first": ""}_${suffix(o)}(
    const unicoap_options_t* options,
    const ${type(o)}* ${o.singular}
) {
    ${getterNonRepeatable(o)}
}`

    let getter = (o, first, _zeroCopy) => `
/**
 * @brief Retrieves the${first ? " first" : ""} \`${o.name}\` option, if present
 * ${docMember()}
 *
 * ${docOpts(o, false)}
 * ${docParamGetNonRepeatable(o)}
 *
 * ${docRetSize(o)}
 * ${docRetNotFound(o)}
 * ${docRetCorrupted(o)}
 */
static inline ssize_t unicoap_options_get${first ? "_first": ""}_${suffix(o)}(
    const unicoap_options_t* options,
    ${_zeroCopy ? "const ": ""}${type(o)}* ${o.singular}
) {
    ${getterNonRepeatable(o)}
}`

    let getterNext = (o) => `
/**
 * @brief Gets the next \`${o.name}\` option provided by the specified iterator
 * @memberof unicoap_options_iterator_t
 *
 * @param[in,out] iterator Option iterator
 * @param[out] ${o.singular} Pointer to a variable that will store next ${o.singular}
 *
 * @see @ref unicoap_options_iterator_t::unicoap_options_get_next_by_number
 *
 * ${docRetSize(o)}
 * ${docRetCorrupted(o)}
 * @retval \`-1\` if the iterator is finished
 */
static inline ssize_t unicoap_options_get_next_${suffix(o)}(
    unicoap_options_iterator_t* iterator,
    const ${type(o)}* ${o.singular}
) {
    return unicoap_options_get_next_by_number(iterator, ${constant(o)}, ${cast(o, true, "const ")}${o.singular});
}`

    let getterNextQueryByName = (o, nullTerminated) => `
/**
 * @brief Gets the next \`${o.name}\` option matching the given name, potentially skipping any options in between.
 * @memberof unicoap_options_iterator_t
 *
 * This method splits \`${o.name}\` options at the \`=\` character.
 *
 * @param[in,out] iterator Option iterator
 * @param[in] name Name of query parameter to find${nullTerminated ? ", must be null-terminated" : "\n * @param length Number of UTF-8 code units (bytes) in @p name"}
 * @param[out] value Pointer to a UTF-8 string variable that will store the entire option, including the \`name=\` part
 *
 * ${docRetSize(o)}
 * ${docRetCorrupted(o)}
 * @retval \`-1\` if the iterator is finished
 */
static inline ssize_t unicoap_options_get_next_${suffix(o)}_by_name${nullTerminated ? "_string" : ""}(
    unicoap_options_iterator_t* iterator,
    const char* name,
    ${nullTerminated ? "" : "size_t length,\n    "}const char** value
) {
    return unicoap_options_get_next_query_by_name${nullTerminated ? "_string" : ""}(iterator, ${constant(o)},
        name, ${nullTerminated ? "" : "length, "}value);
}`

        let getterCopyUTF8Joined = (o) => `
/**
 * @brief Copies ${o.contiguousRepresentation} into the given buffer
 * ${docMember()}
 *
 * ${docOpts(o, false)}
 * @param[in,out] ${o.continuous} Buffer of UTF-8 code units
 * ${docParamCapacity(o, o.continuous)}
 *
 * ${docRetLength(o)}
 * ${docRetCorrupted(o)}
 * ${docRetNoBufSCopy(o, o.continuous)}
 * ${o.prefix ? `* @pre @p capacity must be greater than zero\n *` : ""}
 * This function creates a string from all \`${o.name}\` options by joining them with the \`${o.separator}\` separator. ${o.prefix ? `\n * The string will bear the \`${o.prefix}\` prefix even if there is no \`${o.name}\` option.`: ""}
 * The string will not be null-terminated. The resulting string uses UTF-8 encoding.
 */
static inline ssize_t unicoap_options_copy_${suffixPlural(o)}(
   const unicoap_options_t* options,
   ${type(o)} ${o.continuous},
   size_t capacity
) {
    ${o.prefix ? `assert(capacity > 0);
    *${o.continuous} = '${o.prefix}';
    ${o.continuous} += 1;
    capacity -= 1;
    int res = unicoap_options_copy_values_joined(options, ${constant(o)}, ${cast(o)}${o.continuous}, capacity, '${o.separator}');
    return res < 0 ? res : res + 1;` : `return unicoap_options_copy_values_joined(options, ${constant(o)},
        ${cast(o)}${o.continuous}, capacity, '${o.separator}');`}
}`


    let getterQueryByName = (o, nullTerminated) => `
/**
 * @brief Retrieves the first \`${o.name}\` option matching the given
 *        ${nullTerminated ? "null-terminated " : ""}name,if present
 *
 * ${docMember()}
 *
 * This method splits \`${o.name}\` options at the \`=\` character.
 *
 * ${docOpts(o, false)}
 * @param[in] name Name of query parameter to find${nullTerminated ? ", must be null-terminated" : "\n * @param length Number of UTF-8 code units (bytes) in @p name"}
 * @param[out] value Pointer to a UTF-8 string variable that will store the entire option, including the \`name=\` part
 *
 * ${docRetSize(o)}
 * ${docRetCorrupted(o)}
 * @retval \`-1\` if the iterator is finished and no matching \`${o.name}\` option was found
 *
 * @note If you already have an @ref unicoap_options_iterator_t instance allocated somewhere, use
 * it and call @ref unicoap_options_iterator_t::unicoap_options_get_next_${suffix(o)}_by_name instead.
 */
static inline ssize_t unicoap_options_get_first_${suffix(o)}_by_name${nullTerminated ? "_string" : ""}(
    unicoap_options_t* options,
    const char* name,
    ${nullTerminated ? "" : "size_t length,\n    "}const char** value
) {
    unicoap_options_iterator_t iterator;
    unicoap_options_iterator_init(&iterator, options);
    return unicoap_options_get_next_${suffix(o)}_by_name${nullTerminated ? "_string" : ""}(&iterator,
        name, ${nullTerminated ? "" : "length, "}value);
}`

    let setter = (o) => `
/**
 * @brief ${o.repeatable ?  "Adds" :"Sets the"} \`${o.name}\` option
 * ${docMember()}
 *
 * ${docOpts(o, true)}
 * ${docParamSetNonRepeatable(o)}
 *
 * ${docRetZero(o)}
 * ${docRetNoBufSSet(o)}
 */
static inline ssize_t unicoap_options_${op(o)}_${suffix(o)}(
   unicoap_options_t* options,
   ${type(o)} ${o.singular}${sizeParam(o)}
) {
    ${_set(o)}
}`

    let setterUTF8 = (o) => `
/**
 * @brief ${o.repeatable ?  "Adds" :"Sets the"} \`${o.name}\` option
 * ${docMember()}
 *
 * ${docOpts(o, true)}
 * ${docParamSetNonRepeatable(o)} as buffer of UTF-8 code units
 * ${docParamSize(o, o.singular)}
 *   in bytes without potential null-terminator
 *
 * ${docRetZero(o)}
 * ${docRetNoBufSSet(o)}${docPre(o, "@p " + o.singular)}
 */
static inline ssize_t unicoap_options_${op(o)}_${suffix(o)}(
    unicoap_options_t* options,
    ${type(o)} ${o.singular}${sizeParam(o)}
) {
    ${assertPre(o)}${_set(o)}
}`

    let setterUTF8NullTerminated = (o) => `
/**
 * @brief ${o.repeatable ?  "Adds" :"Sets the"} \`${o.name}\` option from null-terminated string
 * ${docMember()}
 *
 * ${docOpts(o, true)}
 * ${docParamSetNonRepeatable(o)} as buffer of UTF-8 code units, must be null-terminated
 *
 * ${docRetZero(o)}
 * ${docRetNoBufSSet(o)}${docPre(o, "@p " + o.singular)}
 *
 * Uses \`strlen\` to calculate the number of code units.
 * @see @ref unicoap_options_t::unicoap_options_${op(o)}_${suffix(o)}
 */
static inline ssize_t unicoap_options_${op(o)}_${suffix(o)}_string(
    unicoap_options_t* options,
    ${type(o)} ${o.singular}
) {
    return unicoap_options_${op(o)}_${suffix(o)}(options, ${o.singular}, strlen(${o.singular}));
}`

        let setterUTF8Joined = (o) => `
/**
 * @brief Adds multiple \`${o.name}\` options from string
 * ${docMember()}
 *
 * ${docOpts(o, true)}
 * @param[in] ${o.continuous} as buffer of UTF-8 code units with values separated by \`${o.separator}\`
 * ${docParamSize(o, o.continuous)} in bytes without potential null-terminator
 *
 * ${docRetZero(o)}
 * ${docRetNoBufSSet(o)}${docPreMax(o, o.plural + "'")}
 *
 * If the string with the values you want to add _starts_ with the separator, you can still use this function.${o.prefix && o.prefix != o.separator ? `\n * However, @p ${o.continuous} must not start with the \`${o.prefix}\` prefix.` : ""}
 */
static inline ssize_t unicoap_options_add_${suffixPlural(o)}(
    unicoap_options_t* options,
    ${type(o)} ${o.continuous}${sizeParam(o)}
) {
    return unicoap_options_add_values_joined(options, ${constant(o)}, ${cast(o)}${o.continuous}, ${sizeParamName(o)}, '${o.separator}');
}`

        let setterUTF8JoinedNullTerminated = (o) => `
/**
 * @brief Adds multiple \`${o.name}\` options from null-terminated string
 * ${docMember()}
 *
 * ${docOpts(o, true)}
 * @param[in] ${o.continuous} as null-terminated string of UTF-8 code units with values separated by \`${o.separator}\`
 *
 * ${docRetZero(o)}
 * ${docRetNoBufSSet(o)}${docPreMax(o, o.plural)}
 *
 * If the string with the values you want to add _starts_ with the separator, you can still use this function.${o.prefix && o.prefix != o.separator ? `\n * However, @p ${o.continuous} must not start with the \`${o.prefix}\` prefix.` : ""}
 * Uses \`strlen\` to calculate the number of code units.
 *
 * @see @ref unicoap_options_t::unicoap_options_add_${suffixPlural(o)}
 */
static inline ssize_t unicoap_options_add_${suffixPlural(o)}_string(
    unicoap_options_t* options,
    ${type(o)} ${o.continuous}
) {
    return unicoap_options_add_${suffixPlural(o)}(options, ${o.continuous}, strlen(${o.continuous}));
}`

    let setterBytes = (o) => `
/**
 * @brief ${o.repeatable ?  "Adds" :"Sets the"} \`${o.name}\` option
 * ${docMember()}
 *
 * ${docOpts(o, true)}
 * ${docParamSetNonRepeatable(o)}
 * ${docParamSize(o, o.singular)} in bytes
 *
 * ${docRetZero(o)}
 * ${docRetNoBufSSet(o)}${docPre(o, "@p " + o.singular)}
 */
static inline ssize_t unicoap_options_${op(o)}_${suffix(o)}(
    unicoap_options_t* options,
    ${type(o)} ${o.singular}${sizeParam(o)}
) {
    ${assertPre(o)}${_set(o)}
}`

    let remove = (o) => `
/**
 * @brief Removes the \`${o.name}\` option, if present
 * ${docMember()}
 *
 * ${docOpts(o, true)}
 *
 * ${docRetZero(o)}
 */
static inline int unicoap_options_remove_${suffix(o)}(unicoap_options_t* options)
{
    return unicoap_options_remove(options, ${constant(o)});
}`

    let removeAll = (o) => `
/**
 * @brief Removes all \`${o.name}\` options, if any
 * ${docMember()}
 *
 * ${docOpts(o, true)}
 *
 * ${docRetZero(o)}
 */
static inline int unicoap_options_remove_${suffixPlural(o)}(unicoap_options_t* options)
{
    return unicoap_options_remove_all(options, ${constant(o)});
}`

    let handle = (o) => {
        var accessors = []

        const register = (generatorFunc, ...args) => {
          accessors.push(generatorFunc(...args)
            + ` /* generated by '${generatorFunc.name}' template */`);
        };

        if (o.repeatable) {
            if (o.format == "bool" || o.format == "block") {
                console.error(`option ${o.name} cannot be repeatable`)
            }

            switch (o.format) {
                case "utf8":
                    register(getterUTF8, o, true);
                    register(getterNext, o, true);
                    if (o.supportsQueryByName) {
                        register(getterNextQueryByName, o, false);
                        register(getterNextQueryByName, o, true);
                        register(getterQueryByName, o, false);
                        register(getterQueryByName, o, true);
                    }
                    register(getterCopyUTF8Joined, o);
                    register(setterUTF8, o);
                    register(setterUTF8NullTerminated, o);
                    register(setterUTF8Joined, o);
                    register(setterUTF8JoinedNullTerminated, o);
                    register(removeAll, o);
                    break
                case "bytes":
                    register(getter, o, true, true);
                    register(getterNext, o, true);
                    register(setterBytes, o);
                    register(removeAll, o);
                    break
                default:
                    register(getter, o, true);
                    register(getterNext, o, true);
                    register(setter, o);
                    register(removeAll, o);
                    break
            }

        } else {
            switch (o.format) {
                case "bool":
                    register(getterBool, o);
                    register(setter, o);
                    break;
                case "utf8":
                    register(getterUTF8, o);
                    register(setterUTF8, o);
                    register(setterUTF8NullTerminated, o);
                    register(remove, o);
                    break
                case "bytes":
                    register(getter, o);
                    register(setterBytes, o);
                    register(remove, o);
                    break
                default:
                    register(getter, o);
                    register(setter, o);
                    register(remove, o);
                    break
            }
        }

        return accessors
    }

    const sectionExtras = {
        "Block-wise Transfers": {
            pre: `
/**
 * @brief Sentinel value for @ref unicoap_block_option_t indicating an uninitialized value
 * @note This constant occupies 4 bytes and does thus not represent a valid \`Block1\` or \`Block2\`
 *   value.
 */
#define UNICOAP_BLOCK_OPTION_NONE (0x0f000000)

/**
 * @brief \`Block1\` and \`Block2\` option value
 */
typedef uint32_t unicoap_block_option_t;

/**
 * @brief Retrieves a \`Block1\` or \`Block2\` option
 * ${docMember()}
 *
 * ${docOpts(OPTIONS["Block1"], false)}
 * @param number Option number (@ref ${constant(OPTIONS["Block1"])} or @ref ${constant(OPTIONS["Block2"])})
 * @param[out] block Pointer to variable that will store \`Block1\` or \`Block2\` value
 *
 * ${docRetSize(OPTIONS["Block1"])}
 * ${docRetNotFound(OPTIONS["Block1"])}
 * ${docRetCorrupted(OPTIONS["Block1"])}
 */
static inline ssize_t unicoap_options_get_block(
    const unicoap_options_t* options,
    unicoap_option_number_t number,
    unicoap_block_option_t* block
) {
    return unicoap_options_get_uint24(options, number, block);
}

/**
 * @brief Sets the \`Block1\` or \`Block2\` option
 * ${docMember()}
 *
 * ${docOpts(OPTIONS["Block1"], true)}
 * @param number Option number (@ref ${constant(OPTIONS["Block1"])} or @ref ${constant(OPTIONS["Block2"])})
 * @param block \`Block1\` or \`Block2\` value
 *
 * ${docRetZero(OPTIONS["Block1"])}
 * ${docRetNoBufSSet(docRetZero(OPTIONS["Block1"]))}
 */
static inline int unicoap_options_set_block(
    unicoap_options_t* options,
    unicoap_option_number_t number,
    unicoap_block_option_t block
) {
    assert(block <= UNICOAP_UINT24_MAX);
    return unicoap_options_set_uint(options, number, block);
}
`,
            post: ""
        },
        "Observe": {
            pre: `
/** @brief Observe value for registering for notifications */
#define UNICOAP_OBSERVE_OPTION_REGISTER   (0)

/** @brief Observe value for cancelling a registration for notifications */
#define UNICOAP_OBSERVE_OPTION_DEREGISTER (1)
`,
            post: `

/**
 * @brief Sets the \`Observe\` option to a randomly generated value
 * ${docMember()}
 *
 * ${docOpts(OPTIONS["Observe"], false)}
 *
 * ${docRetZero(OPTIONS["Observe"])}
 * ${docRetNoBufSSet(OPTIONS["Observe"])}
 */
int unicoap_options_set_observe_generated(unicoap_options_t* options);
`
        }
    }

    let out = ""

    let options = Object.values(OPTIONS)

    let nonRepeatables = options.filter(o => !o.repeatable && !o.section)
    out += nonRepeatables.map(o => section(o.name, handle(o).join("\n"))).join("\n")
    out += "\n"

    let repeatables = options.filter(o => o.repeatable && !o.section)
    out += repeatables.map(o => section(o.name, handle(o).join("\n"))).join("\n")
    out += "\n"

    let withSection = Object.groupBy(options.filter(o => !!o.section), ({ section }) => section);
    out += Object.entries(withSection).map(([name, opts]) =>
        section(name, sectionExtras[name].pre + opts.map(o => handle(o).join("\n")).join("\n") + sectionExtras[name].post)
    ).join("\n")
    return out
})()

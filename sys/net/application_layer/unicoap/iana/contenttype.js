(() => {
  // This script generates a list of C enum cases and doxygen comments for CoAP options
  // if run on
  // https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#content-formats
  //
  // Copy and run this script in your web inspector, then copy into sys/include/net/unicoap/constants.h
  // into the unicoap_option_number_t enum.
  //
  // The script also adds cross-references to the documentation comment.
  const casePrefix = "UNICOAP_FORMAT_"
  const enumCaseName = (c) => `${
    casePrefix
  }${
    c.name
  }${
    c.type.includes("cose-type=\"cose-") ? 
      ("_" + c.type.replace(/"/g, "").replace("application/cose; cose-type=cose-", "").toUpperCase()) : 
      ""
  }${
    c.type.includes("application/pkcs7-mime; smime-type=") ? 
      ("_" + c.type.replace("application/pkcs7-mime; smime-type=", "").replace(/-/g, "_").toUpperCase()) : 
      ""
  }${
    c.type.includes("application/aif") && c.type.includes("toid=") ? 
      ("_" + c.type.split("toid=")[1].split(";")[0].replace(/-/g, "_").toUpperCase()) : 
      ""
  }${
    c.type.includes("application/aif") && c.type.includes("tperm=") ? 
      ("_" + c.type.split("tperm=")[1].split(";")[0].replace(/-/g, "_").toUpperCase()) : 
      ""
  }${
    c.type.includes("application/yang-data+cbor; id=") ? 
      ("_" + c.type.replace("application/yang-data+cbor; id=", "").replace(/-/g, "_").toUpperCase()) : 
      ""
  }${
    c.type.includes("application/eat+cwt; eat_profile=\"tag:psacertified.org,2019") ? "_PSA2019" : ""
  }${
    c.type.includes("application/eat+cwt; eat_profile=\"tag:psacertified.org,2023") ? "_PSA2023" : ""
  }${
    c.type.includes("application/eat+cwt; eat_profile=2.16") ? "_PROFILE" : ""
  }${
    c.type.includes("profile") ? "_PROFILE" : ""
  }${
    c.codingSuffix
  }`

  const enumCase = (c) => `    /**
     * @brief Content type \`${c.type}\`${c.codingNote}
     *
     * Media type: ${c.mediaType ? `[\`${c.mediaType}\`](${c.mediaTypeLink})` : "-"}
${c.links.join("\n")}
     */
    ${enumCaseName(c)} = ${c.value},
`

  const formats = [...document.querySelectorAll("#table-content-formats > tbody > tr")]
  .map(e => {
      return {
          type: e.querySelector("td:first-child").textContent,
          coding: e.querySelector("td:nth-child(2)").textContent,
          mediaType: e.querySelector("td:nth-child(3) a")?.textContent,
          mediaTypeLink: e.querySelector("td:nth-child(3) a")?.href,
          value: e.querySelector("td:nth-child(4)").textContent,
          references: [...e.querySelectorAll("td:nth-child(5) a")]
      }
  })
  .filter(c => c.type != "Unassigned" && !c.type.startsWith("Reserved"))
  .map(c => {
    let links = c.references.map(r => `     * @see [${r.textContent}](${r.href})`)

    let name = c.type
        .split(";")[0]
        .split("(")[0]
        .replace("application/", "")
        .replace("vnd.", "")
        .replace("text/plain", "text")
        .replace("link-format", "link")
        .replace(/\s+|-|\+|\/|\./g, "_")
        .toUpperCase()

    let codingSuffix = ""

    let codingNote = ""
    if (c.coding) {
        codingSuffix += "_" + c.coding.toUpperCase()
        codingNote = ` in \`${c.coding}\` coding`
    }
    return {...c, links, name, codingNote, codingSuffix}
  })

  const enumeration = `/**
 * @brief \`Content-Format\` option values
 *
 * @see [Content-Format values assigned by IANA](https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#content-formats)
 */
typedef enum {
`
+ formats.map(enumCase).join("\n") 
+ `} __attribute__((__packed__)) unicoap_content_format_t;

`

  const formatBaseHelper = (name) => `/**
 * @brief Determines whether the given \`Content-Format\` is based on ${name.toUpperCase()}
 * @param format \`Content-Format\` value
 * @return A boolean value indicating whether the format is ${name.toUpperCase()}-based
 */
static inline bool unicoap_content_format_is_${name}(unicoap_content_format_t format) {
    return
${formats.filter(c => c.type.includes(name)).map(c => `      format == ${enumCaseName(c)}`).join(" ||\n")};
}

`

  return enumeration 
    + formatBaseHelper("json") + formatBaseHelper("cbor") + formatBaseHelper("xml")
})()

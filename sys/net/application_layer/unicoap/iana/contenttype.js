// https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#content-formats
[...document.querySelectorAll("#table-content-formats > tbody > tr")]
.map(e => {
    return {
        type: e.querySelector("td:first-child").textContent,
        coding: e.querySelector("td:nth-child(2)").textContent,
        value: e.querySelector("td:nth-child(3)").textContent,
        references: [...e.querySelectorAll("td:nth-child(4) a")]
    }
})
.filter(c => c.type != "Unassigned" && !c.type.startsWith("Reserved"))
.map(c => {

    let links = c.references.map(r =>
    ` * @see [${r.textContent}](${r.href})`
    )

    let name = c.type
        .split(";")[0]
        .split("(")[0]
        .replace("application/", "")
        .replace("vnd.", "")
        .replace("text/plain", "text")
        .replace(/\s+|-|\+|\/|\./g, "_")
        .toUpperCase()

    let codingSuffix = ""

    let codingNote = ""
    if (c.coding) {
        codingSuffix += "_" + c.coding.toUpperCase()
        codingNote = ` in \`${c.coding}\` coding`
    }

return `/**
 * @brief Content type \`${c.type}\`${codingNote}
${links.join("\n")}
 */
UNICOAP_FORMAT_${name}${codingSuffix} = ${c.value},
`
})
.join("\n")

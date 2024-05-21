function initMenu(relPath,searchEnabled,serverSide,searchPage,search) {
    function makeTree(data,relPath,dropdown=false) {
        var result='';
        if ('children' in data) {
            if (dropdown) {
                result += '<ul class="dropdown-menu">';
            }
            for (var i in data.children) {
                var active = '';
                if (location.pathname.endsWith(relPath + data.children[i].url) ||
                    (location.pathname.endsWith("/") &&
                     (location.pathname + "index.html").endsWith(relPath + data.children[i].url))) {
                    console.log(location.pathname)
                    active = ' class="active"';
                }
                result += '<li'+active+'><a href="' + relPath+data.children[i].url+'">' +
                          data.children[i].text;
                if ('children' in data.children[i]) {
                    result += '<span class="caret"></span>'
                }
                result += '</a>' + makeTree(data.children[i],relPath,true) + '</li>';
            }
            if (dropdown) {
                result += '</ul>';
            }
        }
        return result;
    }

    /* code adapted from original menu.js of Doxygen, but made
     * bootstrap ready via
     * http://vadikom.github.io/smartmenus/src/demo/bootstrap-navbar.html */
    $('#riot-navlist').append(makeTree(menudata,relPath));
    if (searchEnabled) {
        $("#MSearchBox").remove();
        if (serverSide) {
            $('#riot-searchform').attr("id", "FSearchBox");
            $('#FSearchBox').attr("action", searchPage);
            $('#FSearchBox').attr("method", "get");
            $('#FSearchBox div.form-group').append('<div id="MSearchBox" class="MSearchBoxInactive"><div class="input-group"><div class="input-group-addon"><span id="MSearchSelect" class="glyphicon glyphicon-search" aria-hidden="true"></span></div><input class="form-control" type="text" id="MSearchField" name="query" placeholder="'+search+'" accesskey="S" onfocus="searchBox.OnSearchFieldFocus(true)" onblur="searchBox.OnSearchFieldFocus(false)"></form></div></div>');
        } else {
            $('#riot-searchform div.form-group').append('<div id="MSearchBox" class="MSearchBoxInactive"><div class="input-group"><div class="input-group-addon"><span id="MSearchSelect" class="glyphicon glyphicon-search" aria-hidden="true" onmouseover="return searchBox.OnSearchSelectShow()" onmouseout="return searchBox.OnSearchSelectHide()"></span></div><input class="form-control" name="q" type="text" id="MSearchField" placeholder="'+search+'" accesskey="S" onfocus="searchBox.OnSearchFieldFocus(true)" onblur="searchBox.OnSearchFieldFocus(false)" onkeyup="searchBox.OnSearchFieldChange(event); modSearch();"/><a id="MSearchClose" href="javascript:searchBox.CloseResultsWindow()"><span id="search-reset" class="glyphicon glyphicon-remove-circle" aria-hidden="true"></span></a></div></div>');
        }
    }
}

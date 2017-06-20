/*
 * Source: http://stackoverflow.com/a/16972927
 */
/* hack to cope with doxygens usage of an older jQuery version, while bootstrap
 * requires a newer one */
jQuery.browser = {};
(function () {
    jQuery.browser.msie = false;
    jQuery.browser.version = 0;
    if (navigator.userAgent.match(/MSIE ([0-9]+)\./)) {
        jQuery.browser.msie = true;
        jQuery.browser.version = RegExp.$1;
    }
})();

/*
 * Copyright (C) 2017 Freie Universität
 *
 * Distributed under terms of the LGPLv2.1 license (see LICENSE)
 */

if ( $("#navrow1").length ) {
    $("#MSearchBox").first(function(index) {
        $(this).remove();
    });

    $("#navrow1 ul.tablist li").each(function(index) {
        var element = $(this).detach();
        if (element.text().trim() == "") {
            return;
        }
        if (element.attr("class") == "current") {
            element.attr("class", "active");
        }
        $("#riot-navlist").append(element);
    });
    $("#navrow1").remove()

    var prev_element = $("nav.navbar")
    for (i = 2; i < 5; i++) {
        $("#navrow" + i + " ul.tablist").each(function(index) {
            var element = $(this).detach();
            element.attr("class", "nav nav-tabs nav-tabs-regs");
            element.children("li.current").each(function(i) {
                $(this).attr("class", "active");
            });
            prev_element.after(element);
            prev_element = element;
        });
        $("#navrow" + i)
    }
}

$(".image > img").addClass("img-responsive")

function resize_content(sidenav)
{
    var side_nav_width = sidenav.outerWidth();
    $("#doc-content").css({marginLeft:parseInt(side_nav_width)+"px"});
}

function resize_handler()
{
    var sidenav = $("#side-nav");
    if ($(window).width() < 750) {
        var toc = $(".toc");
        if (window_before >= 750) {
            original_sidenav_width = sidenav.width();
        }
        sidenav.width("0px");
        sidenav.css("padding-right", "0px");
        sidenav.children(".ui-resizable-e").width("0px");
        sidenav.hide();
        toc.removeClass("toc")
        toc.addClass("toc-sm")
    }
    else {
        var toc = $(".toc-sm");
        sidenav.width(parseInt(original_sidenav_width)+"px");
        sidenav.css("padding-right", original_padding);
        sidenav.children(".ui-resizable-e").width(original_padding);
        sidenav.show();
        toc.removeClass("toc-sm")
        toc.addClass("toc")
    }
    resize_content(sidenav)
    window_before = $(window).width()
}

$(document).ready(function() {
    var sidenav = $("#side-nav");
    original_padding = sidenav.css("padding-right")
    window_before = $(window).width()
    original_sidenav_width = (window_before >= 740) ? sidenav.width() : 275;
    resize_handler();
});
$(window).resize(resize_handler);

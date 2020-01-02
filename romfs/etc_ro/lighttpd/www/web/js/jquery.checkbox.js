/*!
 * jQuery Checkbox Plugin v1.0.0
 *
 * Copyright 2013 jumi
 * Released under the MIT license
 */
(function(a){function d(e){var c='<label for="'+e.attr("id")+'"></label>',d='<span class="chkbox_disabled">'+I18N("j","Disabled")+'</span><span class="chkbox_enabled">'+I18N("j","Enabled")+"</span>",c=a(c);c.on(f,function(b){a(this).addClass("active")});c.on(g,function(b){b.preventDefault();a(this).removeClass("active")});c.on(h,function(b){b.preventDefault();b=a(this).attr("for");b=a("#"+b);b.prop("checked")?a(this).css("animation","checkboxclose 0.2s"):a(this).css("animation","checkboxopen 0.2s");
b.trigger("click")});e.addClass("chkbox");e.after(c.append(d))}var f="mousedown",g="mouseup mouseout",h="click";a(document).ready(function(){"undefined"!=typeof jQuery.mobile&&1==jQuery.mobile.support.touch&&(h="tap",f="touchstart",g="touchend")});a.fn.checkbox=function(){d(this)}})(jQuery);

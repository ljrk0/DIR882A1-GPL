/* svn info: $Revision: 1003 $ $Date: 2018-08-06 17:53:29 +0800 (Mon, 06 Aug 2018) $ */
/*!
 * jQuery Checkbox Plugin v1.0.0
 *
 * Copyright 2013 jumi
 * Released under the MIT license
 */
!function(o){var n="mousedown",s="mouseup mouseout",c="click";o(document).ready(function(){void 0!==jQuery.mobile&&1==jQuery.mobile.support.touch&&(c="tap",n="touchstart",s="touchend")}),o.fn.checkbox=function(){var e,t,a;t='<label for="'+(e=this).attr("id")+'"></label>',a='<span class="chkbox_disabled">'+I18N("j","Disabled")+'</span><span class="chkbox_enabled">'+I18N("j","Enabled")+"</span>",(t=o(t)).on(n,function(e){o(this).addClass("active")}),t.on(s,function(e){e.preventDefault(),o(this).removeClass("active")}),t.on(c,function(e){e.preventDefault(),e=o(this).attr("for"),(e=o("#"+e)).trigger("click")}),e.addClass("chkbox"),e.after(t.append(a))}}(jQuery);
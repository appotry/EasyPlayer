/**
 * 文字溢出,裁剪掉尾部文字,拼接...鼠标悬停用浮层显示
 */
;
(function($) {
	$.fn.ellipsis = function() {

		this.css({
			'overflow' : 'hidden',
			'text-overflow' : 'ellipsis',
			'white-space' : 'nowrap'
		});

		this.bind('mouseenter', function() {
			var $this = $(this);
			if (this.offsetWidth < this.scrollWidth) {
				$this.attr('title', $this.text());
			}
		});
	};
})(jQuery);
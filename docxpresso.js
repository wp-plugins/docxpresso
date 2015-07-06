jQuery(function($) {
    $(document).ready(function(){
            $('#insert-docxpresso').click(open_docxpresso_window);
        });
    function open_docxpresso_window() {
		if (this.window === undefined) {
			this.window = wp.media({
					title: 'Insert a document',
					library: {type: 'application/vnd.oasis.opendocument.text'},
					multiple: false,
					button: {text: 'Insert'}
				});
	 
			var self = this; // Needed to retrieve our variable in the anonymous function below
			this.window.on('select', function() {
					var first = self.window.state().get('selection').first().toJSON();
					for (attr in first)
						console.log(attr + '->' + first[attr]);
					if (first.subtype == 'vnd.oasis.opendocument.text') {
						wp.media.editor.insert('[docxpresso file="' + first.url + '"]');
					} else {
						window.alert('You nee to use a .odt file');
					}
				});
		}
	 
		this.window.open();
		return false;
	}
});
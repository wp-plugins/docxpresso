<?php

/*
  Plugin Name: Docxpresso
  Plugin URI: http://www.docxpresso.com/plugins/wp-docxpresso
  Description: Insert content coming from an Office file (.odt)
  Version: 1.2
  Author: No-nonsense Labs
  Author URI: http://www.docxpresso.com
 */

if (is_admin()) {
    include dirname(__FILE__) . '/admin.php';
} else {

    function docxpresso_call($attrs, $content = null) {

        $buffer = '';
		if (isset($attrs['file'])) {
            $file = strip_tags($attrs['file']);
			$path = parse_url($file, PHP_URL_PATH);
			$fullPath = $_SERVER['DOCUMENT_ROOT'] . $path;
			require_once 'CreateDocument.inc';

			$doc = new Docxpresso\createDocument(array('template' => $fullPath));
			$html = $doc->ODF2HTML5('test.html', array('format' => 'single-file', 'download' => true, 'parseLayout' => false));
			$buffer = $html;
                        unset($doc);
        }
		return $buffer;
    }

    add_shortcode('docxpresso', 'docxpresso_call');
}

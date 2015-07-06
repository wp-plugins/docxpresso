<?php

/**
 * ODF2HTML5
 *
 * @copyright  Copyright(c) No-nonsense Labs (http://www.nononsenselabs.com)
 */

/*
Plugin Name: Docxpresso
Plugin URI: http://www.docxpresso.com
Description: Docxpresso inserts content from a document file (.odt).
Version: 1.0
Author: No-nonsense Labs
License: GPLv2 or later
*/

namespace Docxpresso\ODF2HTML5;


use Docxpresso;


class ODF2HTML5
{
    
    /**
     * Stores information about the stroke scaling factor for rendering SVGs
     * 
     * @var array
     * @access public
     * @static
     */
    public static $strokeScales = array();
    /**
     * Stores background images for SVG rendering
     * 
     * @var array
     * @access private
     */
    private $_backImages;
    /**
     * HTML5 body element
     * 
     * @var DOMDocument
     * @access private
     */
    private $_body;
    /**
     * The JS engine used to render charts
     * 
     * @var string
     * @access private
     */
    private $_chartJS;
    /**
     * Stores chart names
     * 
     * @var array
     * @access private
     */
    private $_charts;
    /**
     * Generated CSS string
     * 
     * @var string
     * @access private
     */
    private $_CSS;
    /**
     * The current form name
     * 
     * @var int
     * @access private
     */
    private $_currentForm;
    /**
     * The current master style
     * 
     * @var string
     * @access private
     */
    private $_currentMasterStyle;
     /**
     * An integer that counts the parsed nodes
     * 
     * @var int
     * @access private
     */
    private $_currentNode;
    /**
     * The current HTML page layout identifier
     * 
     * @var string
     * @access private
     */
    private $_currentHTMLPage;
    /**
     * sets the default font size
     * 
     * @var string
     * @access private
     */
    private $_defaultFontSize;
    /**
     * Reference to the ODF document
     * 
     * @var CreateDocument
     * @access private
     */
    private $_doc;
    /**
     * Stores all the required DOMDocument objects
     * 
     * @var DOMDocument
     * @access private
     */
    private $_dom;
    /**
     * ending variable delimiter
     * 
     * @var string
     * @access private
     */
    private $_end;
    /**
     * Keeps track of the endnote numbering
     * 
     * @var int
     * @access private
     */
    private $_endnoteCounter;
    /**
     * Stores the endnotes nodes
     * 
     * @var array
     * @access private
     */
    private $_endnotes;
    /**
     * Global page CSS properties
     * 
     * @var string
     * @access private
     */
    private $_externalCSS;
    /**
     * Keeps track of the footnote numbering
     * 
     * @var int
     * @access private
     */
    private $_footnoteCounter;
    /**
     * Stores the footnotes nodes
     * 
     * @var array
     * @access private
     */
    private $_footnotes;
    /**
     * Stores the form info
     * 
     * @var array
     * @access private
     */
    private $_forms;
    /**
     * This array stores the xml of the body child nodes together with the 
     * associated master style
     * 
     * @var array
     * @access private
     */
    private $_HTML;
    /**
     * This array stores all the required info to build the HTML body element:
     * the HTML code, page style, header and footer for each different layout
     * section of the final HTML5 document
     * 
     * @var array
     * @access private
     */
    private $_HTMLStructure;
    /**
     * This array stores the images used in the document
     * 
     * @var array
     * @access private
     */
    private $_images;
    /**
     * This array stores the relationship among master and layout styles
     * 
     * @var array
     * @access private
     */
    private $_masterLayout;
    /**
     * Metadata information
     * 
     * @var mixed
     * @access private
     */
    private $_metadata;
    /**
     * This array stores all different page styles used in the document
     * 
     * @var array
     * @access private
     */
    private $_pageStyle;
    /**
     * This array stores all the required path info for the resulting html
     * 
     * @var array
     * @access private
     */
    private $_path;
    /**
     * This array stores all ODF style inheritance
     * 
     * @var array
     * @access private
     */
    private $_parentStyle;
    /**
     * Determines if charts should be parsed
     * 
     * @var boolean
     * @access private
     */
    private $_parseCharts;
    /**
     * Determines if document layout is parsed
     * 
     * @var boolean
     * @access private
     */
    private $_parseLayout;
    /**
     * prefix used as a namespace for styles
     * 
     * @var string
     * @access private
     */
    private $_prefix;
    /**
     * The path where the resultin HTML file should be saved
     * 
     * @var string
     * @access private
     */
    private $_rawPath;
    /**
     * True if we want the ouput to be a single file
     * 
     * @var bool
     * @access private
     */
    private $_singleFile;
    /**
     * starting varaible delimiter
     * 
     * @var string
     * @access private
     */
    private $_start;
    /**
     * This array stores all styles
     * 
     * @var array
     * @access private
     */
    private $_style;
    /**
     * This array stores the relationship among styles and master styles
     * 
     * @var array
     * @access private
     */
    private $_style2master;
    /**
     * This array stores the child parent relationship among styles
     * 
     * @var array
     * @access private
     */
    private $_style2style;
    /**
     * If true drawings are parsed
     * 
     * @var boolean
     * @access private
     */
    private $_SVG;
    /**
     * An instance of the SVGParser object
     * 
     * @var SVGParser
     * @access private
     */
    private $_SVGParser;
    /**
     * The DOMXPath object obtained from $this->_dom
     * 
     * @var DOMXPath
     * @access private
     */
    private $_xpath;
    
    
    /**
     * Construct
     *
     * @param CreateDocument $doc
     * @param string $path
     * @param array $options
     * @access public
     */
    public function __construct($doc, $path, $options)
    {          
        //initialize variables
        $this->_prefix = 'h5p_' . uniqid() . ' ';
        $this->_rawPath = $path;
        $this->_download = true;
        if (!isset($options['parseLayout']) || $options['parseLayout'] == true) {
            $this->_parseLayout = true;
        } else {
            $this->_parseLayout = false;
        }
        //set global chart parsing defaults
        if (!isset($options['charts']) || $options['charts'] == true) {
            $this->_chartJS = Docxpresso\CreateDocument::$config['charts']['js'];
        } else {
            $this->_chartJS = false;
        }
        if (isset($options['css'])) {
            $CSSPath = $options['css'];
        } else {            
                $CSSPath = __DIR__ . '/default.css';
        }
        //take into account external CSS props
        try {
            $extCSS = \file_get_contents($CSSPath);
            $this->_externalCSS = \str_replace('.h5p_page',
                                               'div#' . \trim($this->_prefix),
                                               $extCSS);
            if (empty($this->_externalCSS)) {
                throw new \Exception('Error while trying to get global styles');
            }
        } catch (\Exception $e) {
            
        }
        if (isset($options['metadata'])) {
            $this->_metadata = $options['metadata'];
        } else {
            $this->_metadata = false;
        }
        $this->_charts = array();
        $this->_defaultFontSize = '';
        $this->_forms = array();
        $this->_HTML = array();
        $this->_HTMLStructure = array();
        $this->_images = array();
        $this->_noteCounter = 0;
        $this->_masterLayout = array();
        $this->_style = array();
        $this->_style2master = array();
        $this->_style2style = array();
        $this->_pageStyle = array();
        $this->_parentStyle = array();   
        if (isset($options['parseCharts'])) {
            $this->_parseCharts = $options['parseCharts'];
        } else {
            $this->_parseCharts = true;
        }
        //SVG parsing
        if (isset($options['parseSVG'])) {
           $this->_SVG = $options['parseSVG'];
        } else {
            $this->_SVG = true;
        }
        if ($this->_SVG){
            $this->_SVGParser = new SVGParser($this);
            $this->_backImages = array();
        }
        $this->_CSS = '';
        $this->_currentHTMLPage = 0;
        $this->_currentMasterStyle = '';
        $this->_currentNode = 0;
        if (isset($options['format']) && $options['format'] == 'single-file') {
            $this->_singleFile = true;
        } else {
            $this->_singleFile = false;
            //we extract the name so we can create the folders containing
            //the images, styles and javascript if any
            $this->_path = \pathinfo($path);
        }
        //get the required document info
        $this->_doc = $doc;
        $this->_dom = $doc->getDOM();
        $this->_xpath = new \DOMXPath($this->_dom['content.xml']);
        //define some required objects to start to build the HTML5 file
        $this->_body = new \DOMDocument();
        $this->_body->loadXML('<body />');
        
        if ($this->_SVG) {
            //look for draw background images
            $bImages = $this->_dom['styles.xml']
                            ->getElementsByTagName('fill-image');
            foreach ($bImages as $img) {
                $imgName = $img->getAttribute('draw:name');
                $path = $img->getAttribute('xlink:href');
                $this->_backImages[$imgName] = $path;
            }
        }
        //build the different HTML5 document portions
        //$this->_generateHead();
        $this->_generateStyle();
        $this->_generateBody();
        /*echo '<body>';
        foreach ($this->_HTML as $key => $value) {
            $node = $value['xml'];
            echo $node->ownerDocument->saveXML($node);
        }
        echo '</body>';*/
        
        //$this->_render();
    }
      
    /*Getters and Setters*/
    
    
    /**
     * Gets the HTML5 body element as a string
     *
     * @return string
     * @access public
     */
    public function getBody() 
    {
        return $this->_body->saveXML();
    }
    
    /**
     * Gets the current HTML page reference
     *
     * @return int
     * @access public
     */
    public function getCurrentHTMLPage() 
    {
        return $this->_currentHTMLPage;
    } 
    
    /**
     * Sets the current HTML page reference
     *
     * @param int $page
     * @return void
     * @access public
     */
    public function setCurrentHTMLPage($page) 
    {
        $this->_currentHTMLPage = $page;
    }
    
    /**
     * Gets the HTML5 style element as a string
     *
     * @return string
     * @access public
     */
    public function getStyle() 
    {
        return $this->_style->saveXML();
    }
    
    /**
     * Some ODF properties have a different behaviour in CSS so we need to
     * filter them to avoid beraking the rendering in a browser
     *
     * @param string $prop
     * @param string $value
     * @param bool $important
     * @return void
     * @access private
     */
    private function _filterProp($prop, $value, $important = false)
    {
        $style = $prop . ': ' . $value;
        if ($important) {
            $style .= ' !important;';
        } else {
            $style .= ';';
        }
        if ($prop == 'margin' && \strpos($value, '%') !== false) {
            $style = '';
        }
        return $style;
    }
    
    /**
     * Takes care of the positioning of image, chart and math elements
     *
     * @param DOMNode $newNode
     * @param DOMNode $frame
     * @return void
     * @access private
     */
    private function _framePositioning($newNode, $frame, $type = 'image')
    {
        
        $style = 'z-index: 100;';
        if ($type == 'image'){
            $width = $frame->getAttribute('svg:width');
            if (!empty($width)) {
                $style .= 'width: ' . $width . ';';
            }
        }
        $height = $frame->getAttribute('svg:height');
        if (!empty($height)) {
            $style .= 'height: ' . $height . ';';
        }
        $width = $frame->getAttribute('svg:width');
        if (!empty($width)) {
            $style .= 'width: ' . $width . ';';
        }
        //check at svg:x for horizontal positioning
        $anchorType = $frame->getAttribute('text:anchor-type');
        if ($anchorType != 'as-char') {
            //$style .= 'display: block';
        } else if ($type != 'object') {
            $style .= 'display: inline';
        }
        $newNode->setAttribute('style', $style);
        if ($type == 'frame') {
            $class= '';
            $class .= $frame->getAttribute('draw:class-names');
            if (!empty($class)) {
                $class .= ' ' . $frame->getAttribute('draw:style-name');
            } else {
                $class .= $frame->getAttribute('draw:style-name');
            }
            if (!empty($class)) {
                $newNode->setAttribute('class', $class);
            }
        }
        //check at svg:x for horizontal positioning
        $class = $frame->getAttribute('draw:style-name');
        $x = $frame->getAttribute('svg:x');
        if (!empty($x)) {
            $offset = self::convertUnits('pt', $x);
            if ($offset > 100) {
                //for frames displaced more than 100pt to the right we assume
                //float: right
                if (isset($this->_style['div.' . $class])
                    && strpos($this->_style['div.' . $class], 'float: left') !== false) {
                    $this->_style['div.' . $class] = 
                            \str_replace('float: left',
                                         'float: right',
                                         $this->_style['div.' . $class]);
                }
            }
        }
    }
    
    /**
     * This method allows to convert Open Document format content into HTML5
     * content. All HTML5 tags store a reference to the original ODF tag element
     * to simplify going back and forth abetween both standards
     *
     * @return void
     * @access private
     */
    private function _generateBody()
    {       
             
        //get the office:text node to start the parsing
        $content = $this->_dom['content.xml'];
        $ns = 'urn:oasis:names:tc:opendocument:xmlns:office:1.0';
        $root = $content->getElementsByTagNameNS($ns, 'text')->item(0);
        $rootChilds = $root->childNodes;
        $body = $this->_body->documentElement;
        $n = 0;
        foreach ($rootChilds as $child) {
            if ($child->nodeType == 1) {
                $parsedNode = $this->_parseODFNode($child, $body, false);
                if (!empty($parsedNode)) {
                    $this->_HTML[$n]['xml'] = $parsedNode;
                    $n++;
                    $this->_currentNode = $n;
                }
            }
        }
        
    }
    
    /**
     * This method generates HTML metadata from the document metadata o from
     * the user supplied metadata
     *
     * @return string
     * @access private
     */
    public function generateMetadata()
    {
        $parsable = array(  'dc:creator' => true,
                            'dc:date' => true,
                            'dc:description' => true,
                            'dc:language' => true,
                            'dc:subject' => true,
                            'dc:title' => true,
                            'meta:auto-reload' => false,
                            'meta:creation-date' => true,
                            'meta:document-statistic' => false,
                            'meta:editing-cycles' => false,
                            'meta:editing-duration' => false,
                            'meta:generator' => false,
                            'meta:hyperlink-behaviour' => false,
                            'meta:initial-creator' => true,
                            'meta:keyword' => true,
                            'meta:print-date' => false,
                            'meta:printed-by' => false,
                            'meta:template' => false, 
                            'meta:user-defined' => true,  
                        );
        $meta = '';
        $metadata = array();
        if (\is_array($this->_metadata)) {
            $metadata = $this->_metadata;
        } else if ($this->_metadata == true) {
            $dom =  new \DOMDocument;
            $dom->loadXML($this->_doc->template['meta.xml']);
            $base = $dom->getElementsByTagName('meta')->item(0);
            $childs = $base->childNodes;
            foreach ($childs as $child) {
                if ($child->nodeName == 'meta:user-defined') {
                    $name = $child->getAttribute('meta:name');
                    $value = $child->nodeValue;
                    $metadata[$name] = $value;
                } else if (isset($parsable[$child->nodeName]) 
                           && $parsable[$child->nodeName]) {
                    $temp = \explode(':', $child->nodeName);
                    $name = \array_pop($temp);
                    $value = $child->nodeValue;
                    $metadata[$name] = $value;
                }
            }
        } else {
            return $meta;
        }
            foreach ($metadata as $name => $content) {
                $meta .= '<meta name="' . $name . '" ';
                $meta .= 'content="' . $content . '">' . PHP_EOL;
            }
        return $meta;
    }
    
    /**
     * This method allows to translate the ODF styles into CSS styles
     *
     * @return void
     * @access private
     */
    private function _generateStyle()
    { 
        //get the default style
        $this->_generateStyleFromSource($this->_dom['styles.xml'], 'styles');
        if ($this->_SVG && isset($this->_style['img'])) {
            //$this->_style['svg'] = $this->_style['img'];   
        }
        //get the automatic styles from style.xml
        $this->_generateStyleFromSource($this->_dom['styles.xml'], 
                                        'automatic-styles');
        //get the master styles from style.xml
        $this->_generateStyleFromSource($this->_dom['styles.xml'], 
                                        'master-styles');
        //get the automatic style nodes from content.xml
        $this->_generateStyleFromSource($this->_dom['content.xml'], 
                                        'automatic-styles');
        
        //we also should update now the info regarding the style2master array
        //to include inheritance recorded in the style2style array
        $this->_updateStyleMasterRelationships($this->_style2style);
    }
    
    /**
     * Translates the styles from a given source
     *
     * @param DOMDocument $src
     * @param string $nodeName
     * @return void
     * @access private
     */
    private function _generateStyleFromSource($src, $nodeName)
    { 
        //get the default style nodes;
        $rootElements = $src->getElementsByTagName($nodeName);
        //let us try to extract the default font size for text if any
        if ($nodeName == 'styles') {
            $xpath = new \DOMXpath($src);
            $query = '//style:default-style[@style:family="paragraph"]';
            $query .= '/style:text-properties';
            $def = $xpath->query($query);
            $length = $def->length;
            if ($length > 0) {
                $defSize = $def->item($length -1)->getAttribute('fo:font-size');
            }       
            if (!empty($defSize)) {
                $this->_defaultFontSize = $defSize;
            }
        }
        if ($rootElements->length > 0) {
            $root = $rootElements->item(0);
            $rootChilds = $root->childNodes;
            foreach ($rootChilds as $child) {
                if($child->nodeType == 1) {
                    $this->_parseODFStyle($child);
                }
            }
        }
    }
    
    /**
     * Generates the path to the image from the ODF sorurce and the options and
     * stores the image
     *
     * @param string $srcODF
     * @param boolean $css
     * @return void
     * @access private
     */
    private function _imagePath($srcODF, $css = false)
    {
        if ($this->_singleFile !== true) {
            //get the image name
            $temp = \explode('/', $srcODF);
            $fileName = \array_pop($temp);
            $path = $this->_path['filename'] . '-img/' . $fileName;
            //check if the folder exists otherwise create it
            if (\file_exists($this->_path['filename'] . '-img') === false) {
                //create the folder
                \mkdir($this->_path['filename'] . '-img');
            }
            //copy there the image data
            $fp = \fopen($path, 'w');
            \fwrite($fp, $this->_doc->template[$srcODF]);
            \fclose($fp);
            if ($css) {
                $path = '../' . $path;
            }
        } else {
            $img = \base64_encode($this->_doc->template[$srcODF]);
            $temp = \explode('.', $srcODF);
            $extension = \array_pop($temp);
            $path = 'data:image/' . $extension . ';base64,';
            $path .= $img;
        }
        
        return $path;
    }
    
    /**
     * This method generates the required h* styles
     *
     * @param int $level
     * @param string $style
     * @return DOMNode
     * @access private
     */
    private function _addHeadingStyle($level, $style)
    {
        $p = 'p.' . $style;
        $span = $p . ' span';
        $h = 'h' . $level . '.' . $style;
        $hSpan = $h . ' span';
        if (isset($this->_style[$p])) {
            $this->_style[$h] = $this->_style[$p];
        } else {
            //generate the style even if empty due to possible inheritance
            $this->_style[$h] = '';
        }
        if (isset($this->_style[$span])) {
            $this->_style[$hSpan] = $this->_style[$span];
        } else {
            //generate the style even if empty due to possible inheritance
            $this->_style[$hSpan] = '';
        }
        //do the same for the parents
        if (isset($this->_style2style[$style])) {
            //explicitely set that teh new heading styles should inehrit
            //properties from their corresponding parent styles
            $parent = $this->_style2style[$style];
            $this->_parentStyle[$h] = 'h' . $level . '.' . $parent;
            $this->_parentStyle[$hSpan] = $this->_parentStyle[$h] . ' span';
            $this->_addHeadingStyle($level, $parent);
        }       
    }
    
    /**
     * This method append the footnotes and endnotes at the the end of the
     * current section
     *
     * @param int $level
     * @param string $style
     * @return DOMNode
     * @access private
     */
    private function _appendNotes($node, $section = 0)
    {
        if (\count($this->_footnotes) > 0){
            //TODO: parse the footnote-sep layout style node
            $sep = $this->_body->createElement('div', ' ');
            $sepStyle = 'margin: 5px 0; width: 33%; border-bottom: 1pt solid #333;';
            $sep->setAttribute('style', $sepStyle);
            $node->appendChild($sep);
            foreach ($this->_footnotes as $note){
                $node->appendChild($note);
            }
        }
        if (\count($this->_endnotes) > 0){
            //TODO: different styling than footnotes?
            $sep = $this->_body->createElement('div', ' ');
            $sepStyle = 'margin: 5px 0; width: 33%; border-bottom: 1pt solid #333;';
            $sep->setAttribute('style', $sepStyle);
            $node->appendChild($sep);
            foreach ($this->_endnotes as $note){
                $node->appendChild($note);
            }
        }
    }

    /**
     * This method updates the master style property of the current $_HTML array
     * entry
     *
     * @param string $style
     * @return DOMNode
     * @access private
     */
    private function _check4MasterStyle($style)
    {
        $n = $this->_currentNode;
        if (isset($this->_style2master[$style])) {
            $this->_HTML[$n]['master'] = $this->_style2master[$style];
            $this->_currentMasterStyle = $this->_style2master[$style];
        } else {
            $this->_HTML[$n]['master'] = $this->_currentMasterStyle;
        }
    }
    
    /**
     * Seraches for paddings in borderless paragraphs
     *
     * @param string $style
     * @return string
     * @access private
     */
    private function _clearPaddings($style)
    {
        if (\strpos($style, "padding") === false){
            return $style;
        } else {
            $regex = '/border:([^;]+);?/i';
            \preg_match($regex, $style, $matches);
            if (!isset($matches[1])) {
                return $this->_removePadding($style);
            } else {
                if (\strpos($matches[1], "none") === false) {
                    return $style;
                } else {
                    return $this->_removePadding($style);
                }
            }
        }
    }
    
    /**
     * This method inserts the HTML node
     *
     * @param DOMNode $odfNode
     * @param DOMNode $htmlNode
     * @param bool $append
     * @return DOMNode
     * @access private
     */
    private function _createHTMLNode($odfNode, $htmlNode, $append = true)
    { 
        //define a boolean variable for enabling tagging
        $tag = true;
        //define the new node container
        $newNode = NULL;
        $odfTag = $odfNode->nodeName;
        $html = $htmlNode->ownerDocument;
        $attr =  $odfNode->attributes;
        switch ($odfTag) {
            case '#text':
                //to preserve correct style inheritance we force all text to be
                //wrapped within a <span> node
                if ($htmlNode->nodeName != 'span'){
                    $span = $html->createElement('span');
                    $htmlNode = $htmlNode->appendChild($span);
                }
                $newNode = $html->createTextNode($odfNode->nodeValue);
                break;
            case 'text:s':
                $num = $odfNode->getAttribute('text:c');
                if (empty($num)) {
                    $num = 1;
                } 
                for ($j=1; $j < $num; $j++){
                    $newNode = $html->createEntityReference('nbsp');
                    $htmlNode->appendChild($newNode);
                }
                $newNode = $html->createEntityReference('nbsp');
                break;
            case 'text:tab':
                $newNode = $html->createEntityReference('emsp');
                $htmlNode->appendChild($newNode);
                break;
            case 'text:a':
                $newNode = $html->createElement('a');
                //we need to check if there is a containing span that
                //incorporates underline-text: none and if so set the
                //style attribute accordingly otherwise the browsers will
                //display the underlining
                $decor = $this->_spanDecoration($odfNode);
                if ($decor == 'none') {
                    $newNode->setAttribute('style', 
                                           'text-decoration: none');
                }
                
                break;
            case 'text:bookmark':
            case 'text:bookmark-start':
                $newNode = $html->createElement('span');
                $id = $odfNode->getAttribute('text:name');
                $newNode->setAttribute('id', $id);
                break;
            case 'text:bookmark-end':               
                $newNode = $htmlNode;
                $append = false;
                $tag = false;
                break;
            case 'text:h':
                $level = $odfNode->getAttribute('text:outline-level');
                if (!empty($level) && $level > 0 && $level < 7){
                    $newNode = $html->createElement('h' . $level);
                    //Like the styles for a ODF header belong to the same family
                    //than normal paragraphs we have to add the corresponding
                    //styles to the $_style array
                    $hStyle = $odfNode->getAttribute('text:style-name');
                    if (!empty($hStyle)) {
                        $this->_addHeadingStyle($level, $hStyle);
                    }
                } else {
                    $newNode = $html->createElement('p');
                }
                //check if the element is the child of a list-item and proceed
                //accordingly
                $this->_parentListStyle($odfNode, $newNode);
                break;
            case 'text:p':
                $newNode = $html->createElement('p');
                //if it is an empty pargraph node we have to insert a &nbsp; so
                //it is rendered in the browser
                $pChilds = $odfNode->hasChildNodes();
                if (!$pChilds){
                    $newNode = $html->createElement('p', '&nbsp;');
                } else {
                    $newNode = $html->createElement('p');
                }
                //check if the element is the child of a list-item and proceed
                //accordingly
                $this->_parentListStyle($odfNode, $newNode);
                break;
            case 'text:span':
                $newNode = $html->createElement('span');
                break;
            case 'text:line-break':
                $newNode = $html->createElement('br');
                break;
            case 'text:section':
                $newNode = $html->createElement('div');
                break;
            case 'table:table':
                $newNode = $html->createElement('table');
                break;
            case 'table:table-column':
                $newNode = $html->createElement('col');
                break;
            case 'table:table-columns':
                $newNode = $html->createElement('colgroup');
                break;
            case 'table:table-row':
                $newNode = $html->createElement('tr');
                break;
            case 'table:table-cell':
                $newNode = $html->createElement('td');
                break;
            case 'text:list':
                $newNode = $html->createElement('ul');
                break;
            case 'text:list-item':
                $newNode = $html->createElement('li');
                //in ODF the lis-item paragraph styles are overwritten by
                //the child paragraph styles so we have to deal with that
                //reverse inheritance that has no CSS counterpart
                $p = $odfNode->firstChild;
                $pStyle = $p->getAttribute('text:style-name');
                $st = 'color: initial; font-family: initial;';
                if (!empty($pStyle)
                    && isset($this->_style['p span'])) {
                    $st .= $this->_style['p span'];
                }
                if (!empty($pStyle)
                    && isset($this->_style['p.' . $pStyle . ' span'])) {
                    $st .= $this->_style['p.' . $pStyle . ' span'];
                }
                if (!empty($pStyle)
                    && isset($this->_style['p.' . $pStyle ])) {
                    $st .= $this->_style['p.' . $pStyle];
                }
                $newNode->setAttribute('style', $st);

                break;
            case 'text:note':
                $newNode = $html->createElement('sup');
                $noteData = $this->_note($odfNode);
                $noteLink = $html->createElement('a', $noteData['ref']);
                $noteLink->setAttribute('href', '#' . $noteData['id']);
                $newNode->appendChild($noteLink);  
                break;
            case 'draw:frame':
                //The frames are not rendered unless they are childs of the
                //following nodes
                $fParents = array('text:p' => true,
                                  'text:h' => true,
                                  'text:a' => true, 
                                  'text:span' => true);
                $parent = $odfNode->parentNode->nodeName;                
                if (isset($fParents[$parent])) {
                    $display = $odfNode->getAttribute('text:anchor-type');
                    if ($display != 'as-char') {
                        $newNode = $html->createElement('div');
                        //let us define the source
                        $this->_framePositioning($newNode, $odfNode, 'frame');
                    } else {
                        $newNode = $html->createElement('span');
                    }
                } else {
                    return NULL;
                }
                break;
            case 'draw:image':
                //we have to take some info from its draw:frame parent node
                $frame = $odfNode->parentNode;
                //first make sure that it is not an "object replacement"
                $objs = $frame->getElementsByTagName('object');
                if ($objs->length > 0) {
                    //we silently leave
                    $newNode = $htmlNode;
                    $append = false;
                    $tag = false;
                    break;
                }
                $newNode = $html->createElement('img');
                //let us define the source
                $srcODF = $odfNode->getAttribute('xlink:href');
                $src = $this->_imagePath($srcODF);
                $newNode->setAttribute('src', $src);
                $this->_framePositioning($newNode, $frame);
                //look for title and desc attributes
                //svg:title maps to the title attribute
                $titNodes = $frame->getElementsByTagName('title');
                if ($titNodes->length > 0) {
                   $title = $titNodes->item(0)->nodeValue; 
                   $newNode->setAttribute('title', $title);
                }
                //svg:desc is mapped to the alt attribute
                $descNodes = $frame->getElementsByTagName('desc');
                if ($descNodes->length > 0) {
                   $desc = $descNodes->item(0)->nodeValue; 
                   $newNode->setAttribute('alt', $desc);
                }
                break;
            case 'draw:object':
                //let us get the object source
                $srcODF = $odfNode->getAttribute('xlink:href');
                $src = $this->_objPath($srcODF);
                if (\is_array($src) && $src['object'] == 'chart') {
                    //we have to take some info from its draw:frame parent node
                    $frame = $odfNode->parentNode;
                    $name = $frame->getAttribute('draw:name');
                    if (empty($name)) {
                        $name = \uniqid();
                    }
                    $newNode = $html->createElement('div', ' ');
                   $opt = array();
                    $opt['name'] = 'chart-' . \uniqid();
                    $newNode->setAttribute('id', $opt['name']);
                    $this->_framePositioning($newNode, $frame, 'object');
                    $opt['js'] = $this->_chartJS;                    
                    $chart = new ChartParser($src['data']);                    
                    $this->_charts[$opt['name']] = $chart->render($opt);
                } else {
                    //we silently leave
                    $newNode = $htmlNode;
                    $append = false;
                    $tag = false;
                    break;
                }
                break;
            case 'text:table-of-content':
                $newNode = $html->createElement('div');
                break;
            case 'form:form':
                $newNode = $html->createElement('form');
                break;
            case 'draw:control':
                $control = $odfNode->getAttribute('draw:control');
                $setWidth = true;
                if (isset($this->_forms[$control])) {
                    $form =& $this->_forms[$control];
                    if ($form['tag'] == 'datalist') {
                        $newNode = $html->createElement('input');
                        $newNode->setAttribute('name', $form['name']);
                        $newNode->setAttribute('list', $form['name']);
                        $newNode->setAttribute('value', $form['value']);
                        $newNode->setAttribute('form', $form['form']);
                        $dataList = $html->createElement($form['tag']);
                        $dataList->setAttribute('id', $form['name']);
                        $htmlNode->appendChild($dataList);
                    } else {
                        $newNode = $html->createElement($form['tag']);
                        $newNode->setAttribute('id', $form['name']);
                        $newNode->setAttribute('name', $form['name']);
                        if (isset($form['type'])) {
                        $newNode->setAttribute('type', $form['type']);
                        }
                        $newNode->setAttribute('value', $form['value']);
                        $newNode->setAttribute('form', $form['form']);
                        }
                }
                if ($form['tag'] == 'textarea' 
                    || $form['tag'] == 'label'
                    || $form['tag'] == 'button') {
                    if (empty($form['value'])) {
                        //to avoid empty textarea node
                        $form['value'] = ' ';
                    }
                    $newNode->nodeValue = $form['value'];
                } else if ($form['tag'] == 'datalist') {
                    foreach ($form['options'] as $opt){
                        $option = $html->createElement('option');
                        $option->setAttribute('value', $opt);
                        $dataList->appendChild($option);
                    }
                    //change the $form['tag'] value to style the associated
                    //input element
                    $form['tag'] = 'input';
                } else if ($form['tag'] == 'select') {
                    if ($form['multi'] == "true") {
                        $newNode->setAttribute('multiple', 'true');
                    }
                    foreach ($form['options'] as $opt){
                        $option = $html->createElement('option', $opt[0]);
                        $option->setAttribute('value', $opt[1]);
                        if ($opt[2] == "selected") {
                            $option->setAttribute('selected', 'selected');
                        }
                        $newNode->appendChild($option);
                    }
                } else if ($form['type'] == 'checkbox') {
                    $setWidth = false;
                    if ($form['state'] == 'checked'
                        || $form['curState'] == 'checked') {
                        $newNode->setAttribute('checked', 'checked');
                    }
                    if (!empty($form['label'])) {
                        $formLabel = $html->createElement('label', 
                                                          $form['label']);
                    }
                }  else if ($form['type'] == 'radio') {
                    $setWidth = false;
                    if ($form['selected'] == 'true'
                        || $form['curSelected'] == 'true') {
                        $newNode->setAttribute('checked', 'checked');
                    }
                    if (!empty($form['label'])) {
                        $formLabel = $html->createElement('label', 
                                                          $form['label']);
                    }
                } else if ($form['type'] == 'image') {
                    $src = $this->_imagePath($form['image']);
                    $newNode->setAttribute('src', $src);
                }
                //we need to parse here the styles because we need to clone
                //graphic and text styles and also take into account the height
                //only for textareas
                $graph = $odfNode->getAttribute('draw:style-name');
                if (isset($this->_style['div.' . $graph . ' *'])) {
                    $this->_style[$form['tag'] . '.' . $graph] = 
                            $this->_style['div.' . $graph . ' *'];
                }
                $class = $graph . ' ';
                $text = $odfNode->getAttribute('draw:text-style-name');
                if (isset($this->_style['p.' . $text . ' span'])) {
                    $this->_style[$form['tag'] . '.' . $text] = 
                            $this->_style['p.' . $text . ' span'];
                }
                $class .= $text;
                //check for labels and get their style
                if (isset($formLabel) 
                    && isset($this->_style['p.' . $text . ' span'])) {
                    $formLabel->setAttribute('style', 
                                      $this->_style['p.' . $text . ' span']);
                }
                //also run for the select and datalist options
                if ($form['tag'] == 'select' || $form['tag'] == 'select'){
                    if (isset($this->_style['p.' . $text . ' span'])) {
                        $optNodes = $newNode->childNodes;
                        foreach ($optNodes as $optNode) {
                            $optNode->setAttribute('style', 
                                      $this->_style['p.' . $text . ' span']);
                        }
                    }
                }
                //set the generic class tag
                $newNode->setAttribute('class', $this->_rep($class));
                $style = '';
                $width = $odfNode->getAttribute('svg:width');
                if (!empty($width) && $setWidth) {
                    $style .= 'width: ' .  $width .';';
                }
                $height = $odfNode->getAttribute('svg:height');
                if (!empty($height) && $form['tag'] == 'textarea') {
                    $style .= 'height: ' .  $height .';';
                }
                $anchorType = $odfNode->getAttribute('text:anchor-type');
                if ($anchorType != 'as-char') {
                    $style .= 'display: block';
                } else {
                    $style .= 'display: inline; float: none !important';
                }
                if (!empty($style)) {
                    $newNode->setAttribute('style', $style);
                }
                break;
            case 'form:formatted-text':
                $odfTag = 'form:text';
            case 'form:number':               
            case 'form:text':
            case 'form:password':
            case 'form:file':
            case 'form:image':
            case 'form:date':
            case 'form:time':
            case 'form:checkbox':
            case 'form:radio':
                $type = \substr($odfTag, 5);
                $input_name = $odfNode->getAttribute('form:name');
                $control_id = $odfNode->getAttribute('form:id');
                //overwrite it if the xml:id attribute is defined
                $control_id = $odfNode->getAttribute('xml:id');
                $value = $odfNode->getAttribute('form:value');
                $state = $odfNode->getAttribute('form:state');
                $curState = $odfNode->getAttribute('form:current-state');
                $selected = $odfNode->getAttribute('form:selected');
                $curSelected = $odfNode->getAttribute('form:current-selected');
                $image = $odfNode->getAttribute('form:image-data');
                $label = $odfNode->getAttribute('form:label');
                $this->_forms[$control_id] = array();
                $form =& $this->_forms[$control_id];
                $form['form'] = $this->_currentForm;
                $form['tag'] = 'input';
                $form['type'] = $type;
                $form['name'] = $input_name;
                $form['value'] = $value;
                $form['state'] = $state;
                $form['curState'] = $curState;
                $form['selected'] = $selected;
                $form['curSelected'] = $curSelected;
                $form['image'] = $image;
                $form['label'] = $label;
                if ($type == 'text') {
                    //we need to check if it is multiline to overwrite
                    //certain values
                    $query = './form:properties/form:property';
                    $query .= '[@form:property-name="MultiLine"]';
                    $multiline = $this->_xpath->query($query, $odfNode);
                    if ($multiline->length > 0) {
                       $form['tag'] = 'textarea'; 
                    }
                }
                $newNode = $htmlNode;
                $append = false;
                $tag = false;
                break;
            case 'form:textarea':
                $input_name = $odfNode->getAttribute('form:name');
                $control_id = $odfNode->getAttribute('form:id');
                //overwrite it if the xml:id attribute is defined
                $control_id = $odfNode->getAttribute('xml:id');
                $value = $odfNode->getAttribute('form:value');
                $this->_forms[$control_id] = array();
                $form =& $this->_forms[$control_id];
                $form['form'] = $this->_currentForm;
                $form['tag'] = 'textarea';
                $form['name'] = $input_name;
                $form['value'] = $value;
                $newNode = $htmlNode;
                $append = false;
                $tag = false;
                break;
            case 'form:fixed-text':
                $input_name = $odfNode->getAttribute('form:name');
                $control_id = $odfNode->getAttribute('form:id');
                //overwrite it if the xml:id attribute is defined
                $control_id = $odfNode->getAttribute('xml:id');
                $value = $odfNode->getAttribute('form:value');
                $this->_forms[$control_id] = array();
                $form =& $this->_forms[$control_id];
                $form['form'] = $this->_currentForm;
                $form['tag'] = 'label';
                $form['name'] = $input_name;
                $form['value'] = $value;
                $newNode = $htmlNode;
                $append = false;
                $tag = false;
                break;
            case 'form:combobox':
                $input_name = $odfNode->getAttribute('form:name');
                $control_id = $odfNode->getAttribute('form:id');
                //overwrite it if the xml:id attribute is defined
                $control_id = $odfNode->getAttribute('xml:id');
                $value = $odfNode->getAttribute('form:value');
                $this->_forms[$control_id] = array();
                $form =& $this->_forms[$control_id];
                $form['form'] = $this->_currentForm;
                $form['tag'] = 'datalist';
                $form['name'] = $input_name;
                $form['value'] = $value;
                //let us get now the datalist options
                $form['options'] = array();
                $itemNodes = $odfNode->getElementsByTagName('item');
                foreach ($itemNodes as $item) {
                    $label = $item->getAttribute('form:label'); 
                    $form['options'][] = $label;
                }
                $newNode = $htmlNode;
                $append = false;
                $tag = false;
                break;
            case 'form:listbox':
                $input_name = $odfNode->getAttribute('form:name');
                $control_id = $odfNode->getAttribute('form:id');
                //overwrite it if the xml:id attribute is defined
                $control_id = $odfNode->getAttribute('xml:id');
                $value = $odfNode->getAttribute('form:value');
                $this->_forms[$control_id] = array();
                $form =& $this->_forms[$control_id];
                $form['form'] = $this->_currentForm;
                $form['tag'] = 'select';
                $form['name'] = $input_name;
                $form['value'] = $value;
                //check if it is multivalued
                $form['multi'] = $odfNode->getAttribute('form:multiple');
                //let us get now the select options
                $form['options'] = array();
                $itemNodes = $odfNode->getElementsByTagName('option');
                foreach ($itemNodes as $item) {
                    $label = $item->getAttribute('form:label'); 
                    $val = $item->getAttribute('form:value'); 
                    $sel = $item->getAttribute('form:selected');
                    $curSel = $item->getAttribute('form:current-selected');
                    if ($sel == 'true' || $curSel == 'true') {
                        $selected = 'selected'; 
                    } else {
                        $selected = '';
                    }
                    $form['options'][] = array($label, $val, $selected);
                }
                $newNode = $htmlNode;
                $append = false;
                $tag = false;
                break;
            case 'form:button':
                $input_name = $odfNode->getAttribute('form:name');
                $control_id = $odfNode->getAttribute('form:id');
                //overwrite it if the xml:id attribute is defined
                $control_id = $odfNode->getAttribute('xml:id');
                $value = $odfNode->getAttribute('form:label');
                $this->_forms[$control_id] = array();
                $form =& $this->_forms[$control_id];
                $form['form'] = $this->_currentForm;
                $form['tag'] = 'button';
                $form['type'] = 'submit';
                $form['name'] = $input_name;
                $form['value'] = $value;
                $newNode = $htmlNode;
                $append = false;
                $tag = false;
                break;
            case 'form:hidden':
                $input_name = $odfNode->getAttribute('form:name');
                $value = $odfNode->getAttribute('form:value');
                $newNode = $html->createElement('input');
                $newNode->setAttribute('type', 'hidden');
                $newNode->setAttribute('name', $input_name);
                $newNode->setAttribute('value', $value);
                break;
            case 'draw:g':
                $baseNode = $html->createElement('div');                    
                $newNode = $this->_SVGParser->render($baseNode, $odfNode);
                break;
            
            case 'draw:custom-shape':
                $baseNode = $html->createElement('div');                    
                $newNode = $this->_SVGParser->render($baseNode, $odfNode);
                break;
             
            default:
                $newNode = $htmlNode;
                $append = false;
                $tag = false;
        }
        
        if ($append) {
            $htmlNode->appendChild($newNode);
            if (isset($formLabel)) {
                $htmlNode->appendChild($formLabel);
            }
        }
        
        if (($odfNode->hasAttributes() 
             || $odfNode->nodeName == 'text:list-item')
            && !empty($newNode)) {
            $this->_parseODFAttributes($odfNode, $newNode);
        }
        
        return $newNode;
    }
    
    /**
     * Recursively merges parent styles
     *
     * @return void
     * @access private
     */
    private function _mergeParentStyles()
    {
        $num = \count($this->_parentStyle);
        if ($num > 0) {
            foreach ($this->_parentStyle as $key => $value) {
                if (!isset($this->_parentStyle[$value])) {
                    //the parent style has not itself a parent style
                    if (isset($this->_style[$value])) {
                        if (isset($this->_style[$key])) {
                            $this->_style[$key] = $this->_style[$value] 
                                                  . $this->_style[$key];
                        } else {
                            $this->_style[$key] = $this->_style[$value];
                        }
                    }
                    unset($this->_parentStyle[$key]);
                }
            }
            $this->_mergeParentStyles();
        }
    }
    
    /**
     * Parses the footnote/endnote nodes
     *
     * @param DOMNode $node
     * @return void
     * @access private
     */
    private function _note($node)
    {
        $data = array();
        $data['id'] = $node->getAttribute('text:id');
        $data['type'] = $node->getAttribute('text:note-class');
        if ($data['type'] == 'endnote'){
            $this->_endnoteCounter++;
            $ref = self::rowLetter($this->_endnoteCounter);
        } else {
            $this->_footnoteCounter++;
            $ref = $this->_footnoteCounter;
        }
        $cits = $node->getElementsByTagName('note-citation');
        if ($cits->length > 0) {
            $citation = $cits->item(0)->nodeValue;
            if (empty($citation)) {
                $citation = $cits->item(0)->getAttribute('text:label');
            }
            if (empty($citation)) {
                $citation = $ref;
            }
        }
        $data['ref'] = $citation;
        
        $noteBody= $node->getElementsByTagName('note-body');
        if ($noteBody->length > 0) {
            $html = $this->_body->createElement('div');
            $html->setAttribute('id', $data['id']);
            $html->setAttribute('class', 'defaultNote');
            $html->setAttribute('style', 'clear:left');
            $reference = $this->_body->createElement('div');
            $refStyle = 'display:table-cell; font-size: 10pt';
            $reference->setAttribute('style', $refStyle);
            $sup = $this->_body->createElement('sup', $data['ref']);
            $reference->appendChild($sup);
            $html->appendChild($reference);
            $container = $this->_body->createElement('div');
            $contStyle = 'display:table-cell';
            $container->setAttribute('style', $contStyle);
            $childs = $noteBody->item(0)->childNodes;
            foreach ($childs as $child) {
                $this->_parseODFNode($child, $container);
            }
            $html->appendChild($container);
            if ($data['type'] == 'endnote'){
                $this->_endnotes[] = $html;
            } else {
                $this->_footnotes[] = $html;
            }
        } 
        return $data;  
    }
    
    /**
     * Grabs the internal path to the object preparse the data and returns:
     *  NULL: if the object is not to be parsed
     *  an array with the type of object and the xml data
     *
     * @param string $srcODF
     * @return mixed
     * @access private
     */
    private function _objPath($srcODF)
    {
        //TODO: take into account other type of objects not just charts
        //preparse the path
        if (\substr($srcODF, 0, 2) == './') {
            $srcODF = \substr($srcODF, 2);
        }
        if (\substr($srcODF, -1) != '/') {
            $srcODF .= '/';
        }
        $path = $srcODF . 'content.xml';
        
        if (isset($this->_doc->template[$path])) {
            $data = $this->_doc->template[$path];
            //check if it is a chart
            if (\strpos($data, 'office:chart') !== false) {
                return array('object' => 'chart', 'data' => $data);
            }
        } else {
            return NULL;
        }
        return NULL;
    }
    
    /**
     * Parses the text decoration options
     *
     * @param DOMNode $odfNode
     * @param DOMNode $htmlNode
     * @return string
     * @access private
     */
    private function _parentListStyle($odfNode, $htmlNode)
    {
        $parentNode = $odfNode->parentNode;
        $prevSibling = $odfNode->previousSibling;
        $listStyle = 'display: table-cell; margin-left:0 !important;';
        $listStyle .= 'text-indent: 0 !important;';
        if (!empty($parentNode) 
            && $parentNode->nodeName == 'text:list-item'
            && empty($prevSibling)) {
            $htmlNode->setAttribute('style', $listStyle);
            if (isset($this->_style['p span'])){
                $font = self::extractSingleProperty('font-family', 
                                                    $this->_style['p span']);
                $size = self::extractSingleProperty('font-size', 
                                                    $this->_style['p span']);
                $color = self::extractSingleProperty('color', 
                                                    $this->_style['p span']);
            }
        }

    }
    
    /**
     * Parses the background image path
     *
     * @param string $src
     * @return string
     * @access private
     */
    private function _parseBackgroundURL($src)
    {   
        $style = $this->_imagePath($src);
        return 'url(' . $style . ')';
    }
    
    /**
     * Parses the fo:border and fo:border-* because Chrome does not render
     * borders thinner than 1px
     *
     * @param string $name
     * @param string $value
     * @return string
     * @access private
     */
    private function _parseBorders($name, $value)
    {   
        $temp = explode(':', $name);
        $prop = \array_pop($temp);
        $regex = '/([0-9\.\-]+)\s*(px|em|rem|ex|%|in|cm|mm|pt|pc)*/i';
        \preg_match($regex, $value, $matches);
        $data = '';
        if (isset($matches[1]) && isset($matches[2])) {
            $data .= $matches[1];
            $data .= $matches[2];
        }
        if (!empty($data)) {
           $width = self::convertUnits('px', $data);
           if ($width < 1 && $width > 0) {
               $value = \str_replace($data, '1px', $value);
           } else if ($width == 0) {
               return;
           }
        } 
        return $prop . ': ' . $value . ';';
    }
    
    /**
     * Parses the text decoration options
     *
     * @param string $line
     * @param string $data
     * @return string
     * @access private
     */
    private function _parseDecoration ($line, $data)
    {   
        $style = '';
        $lines = array('style:text-underline-style' => 'underline',
                       'style:text-overline-style' => 'overline', 
                       'style:text-line-trough-text-style' => 'line-trough', 
                       );
        if (isset($lines[$line]) && $data != 'none') {
            $style .= 'text-decoration: ' . $lines[$line]. ';';
        } else if ($data == 'none') {
            $style .= 'text-decoration: none !important;';
        }
        $types = array('none' => 'none',
                       'dash' => 'dashed', 
                       'dot-dash' => 'dotted', 
                       'dot-dot-dash' => 'dotted',  
                       'dotted' => 'dotted',  
                       'long-dash' => 'dashed',  
                       'solid' => 'solid',  
                       'wave' => 'wavy',
                       );
        if (isset($types[$data])) {
            $style .= 'text-decoration-style: ' . $types[$data]. ';';
        }
        return $style;
    }
    
    /**
     * Parses the fill SVG property
     *
     * @param string $name
     * @param string $value
     * @return string
     * @access private
     */
    private function _parseFill($name, $value)
    {   
        if ($value == 'none') {
            return 'fill: none;';
        } else if ($value == 'bitmap') {
            return 'fill-opacity: 0;';
        }
    }
    
    /**
     * Parses the headers and footers ODF nodes
     *
     * @param DOMNode $node
     * @param string $master
     * @param string $type
     * @return void
     * @access private
     */
    private function _parseHeaderFooter($node, $master, $type)
    {
        $nodes = $node->getElementsByTagName($type);
        if ($nodes->length > 0) {
            $header = $nodes->item(0);
            $display = $header->getAttribute('style:display');
            if ($display === false || $display == 'false') {
                $style = 'display: none';
            }
            $html = $this->_body->createElement($type);
            if (!empty($style)) {
                $html->setAttribute('style', $style);
            }
            $this->_parseODFNode($header, $html);
            $this->_masterLayout[$master][$type] = $html;
        }
    }
    
    /**
     * Parses the horizontal position property of a frame
     *
     * @param DOMNode $node
     * @return void
     * @access private
     */
    private function _parseHorizPos($node)
    {
        $wrap = $node->getAttribute('style:wrap');
        if ($wrap != 'none') {
            $horpos = $node->getAttribute('style:horizontal-pos');
            if ($horpos == 'right' || $horpos == 'outside') {
                $float = 'right';
            } else if ($horpos == 'center'){
                $style = 'float: none; display: block;';
                $style .= 'margin-left: auto; margin-right: auto;';
                return $style;
            }else {
                $float = 'left';
            }
            return 'float: ' . $float . ';';
        }        
    }
    
    /**
     * Generates a line property in CSS format: width|style|color
     *
     * @param DOMNode $node
     * @return void
     * @access private
     */
    private function _parseLine($node)
    {
        $values = array('color' => '#000000',
                        'width' => '1pt',
                        'style' => 'solid',
                        );
        
        $color = $node->getAttribute('style:color');
        if (empty($color)) {
            $color = $values['color'];
        }
        $style = $node->getAttribute('style:style');
        if (empty($style)) {
            //TODO: check for available CSS values
            $style = $defValues['style'];
        }
        $width = $node->getAttribute('style:width');
        if (empty($width)) {
            $width = $values['width'];
        }
        
        return $width . ' ' . $style . ' ' . $color;
    }
    
    /**
     * Generates a line-height property
     *
     * @param DOMNode $node
     * @return void
     * @access private
     */
    private function _parseLineHeight($name, $value)
    {   
        $temp = explode(':', $name);
        $prop = \array_pop($temp);
        $regex = '/([0-9\.\-]+)\s*(%)*/i';
        \preg_match($regex, $value, $matches);
        if (isset($matches[1]) && isset($matches[2])) {
            $value = $matches[1]/100;
        }

        return $prop . ': ' . $value . ';';
    }
    
    /**
     * This method parses teh attributes of an ODF node and translates tehm into
     * HTML5 attributes
     *
     * @param DOMNode $odfNode
     * @param DOMNode $node
     * @return void
     * @access private
     */
    private function _parseODFAttributes($odfNode, $node)
    { 
        $odfTag = $odfNode->nodeName;
        //check for master styles
        switch ($odfTag) {
            case 'text:a':
                $href = $odfNode->getAttribute('xlink:href');
                $node->setAttribute('href', $href);
                $target = $odfNode->getAttribute('office:target-frame-name');
                $node->setAttribute('target', $target);
                $title = $odfNode->getAttribute('office:title');
                $node->setAttribute('title', $title);
                //TODO: parse text:visited-style-name 
                break;
            case 'text:h':
                $listHeader = $odfNode->getAttribute('text:is-list-header');
                if ($listHeader === true) {
                    //TODO
                }
                $restart = $odfNode->getAttribute('text:restart-numbering');
                if ($restart === true) {
                    $num = $odfNode->getAttribute('text:start-value');
                    //TODO
                }
            case 'text:p':
            case 'text:span':
                $class = "";
                $classes = $odfNode->getAttribute('text:class-names');
                if (!empty($classes)) {
                   $class .= $classes . ' ' ;
                }
                $style = $odfNode->getAttribute('text:style-name');
                if (!empty($style)) {
                   $class .= $style;
                   $this->_check4MasterStyle($style);
                }
                if (!empty($class)) {
                    $node->setAttribute('class', $this->_rep($class));
                }
                $id = $odfNode->getAttribute('xml:id');
                if (!empty($id)) {
                    $node->setAttribute('id', $id);
                }
                //TODO: if the attribute text:list-style-name is set we have
                //to overwrite the list styles with a > p selector of
                //higher specificity
                break;
            case 'text:section':
                $style = $odfNode->getAttribute('text:style-name');
                if (!empty($style)) {
                    $node->setAttribute('class', $this->_rep($style));
                    $this->_check4MasterStyle($style);
                }
                $id = $odfNode->getAttribute('xml:id');
                if (!empty($id)) {
                    $node->setAttribute('id', $id);
                }
                //overwrite the id attribute if the section is named
                $id = $odfNode->getAttribute('text:name');
                if (!empty($id)) {
                    $node->setAttribute('id', $id);
                }
                break;
            case 'table:table-cell':
                $rows = $odfNode->getAttribute('table:number-rows-spanned');
                if (!empty($rows)) {
                    $node->setAttribute('rowspan', $rows);
                }
                $cols = $odfNode->getAttribute('table:number-columns-spanned');
                if (!empty($cols)) {
                    $node->setAttribute('colspan', $cols);
                }
            case 'table:table':
            case 'table:table-row':
                $style = $odfNode->getAttribute('table:style-name');
                if (!empty($style)) {
                    $node->setAttribute('class', $this->_rep($style));
                    $this->_check4MasterStyle($style);
                }
                $id = $odfNode->getAttribute('xml:id');
                if (!empty($id)) {
                    $node->setAttribute('id', $id);
                }
                break;
            case 'table:table-column':
                $st = $odfNode->getAttribute('table:default-cell-style-name');
                if (!empty($st)) {
                    $node->setAttribute('class', $this->_rep($st));
                }
                //overwrite the default style if defined
                $style = $odfNode->getAttribute('table:style-name');
                if (!empty($style)) {
                    $node->setAttribute('class', $this->_rep($style));
                    $this->_check4MasterStyle($style);
                }
                $id = $odfNode->getAttribute('xml:id');
                if (!empty($id)) {
                    $node->setAttribute('id', $id);
                }
                $cols = $odfNode->getAttribute('table:number-columns-repeated');
                if (!empty($cols)) {
                    $node->setAttribute('span', $cols);
                }
                break;
            case 'text:list':
                //TODO: take into account text:continue-list and
                //text:continue-numbering
                $style = $odfNode->getAttribute('text:style-name');
                if (!empty($style)) {
                    $node->setAttribute('class', $this->_rep($style));
                    $this->_check4MasterStyle($style);
                }
                $id = $odfNode->getAttribute('xml:id');
                if (!empty($id)) {
                    $node->setAttribute('id', $id);
                }
                break;
            case 'text:list-item':
                //TODO: take into account text:start-value
                //We have to check if the first child node is a text:list
                //and if so enforce a custom style that removes the unwanted
                //additional bullet
                $firstChildtype = $odfNode->firstChild->nodeName;
                if ($firstChildtype == 'text:list') {
                    $rbf = true;
                } else {
                    $rbf = false;
                }
                $style = $odfNode->getAttribute('text:style-override');
                if (!empty($style)) {
                    if ($rbf) {
                        $node->setAttribute('class', 
                                        $this->_rep($style) . ' removeBullet');        
                    } else {
                        $node->setAttribute('class', $this->_rep($style));
                    }
                    $this->_check4MasterStyle($style);
                } else if ($rbf) {
                    $node->setAttribute('class', 'removeBullet');
                }
                $id = $odfNode->getAttribute('xml:id');
                if (!empty($id)) {
                    $node->setAttribute('id', $id);
                }
                break;
            case 'form:form':
                //get the form name
                $formName = $odfNode->getAttribute('form:name');
                if (empty($formName)) {
                    $formName = self::generateId('form_');
                }
                $this->_currentForm = $formName;
                $node->setAttribute('name', $formName);
                $node->setAttribute('id', $formName);
                //get the form action, method and target
                $action = $odfNode->getAttribute('xlink:href');
                $node->setAttribute('action', $action);
                $method = $odfNode->getAttribute('form:method');
                $node->setAttribute('method', $method);
                $target = $odfNode->getAttribute('office:target-frame');
                $node->setAttribute('target', $target);
                break;
            case 'draw:control':
                //the attributes have to be parsed with the generation of the 
                //node because they may depend on the original form tag
                break;
            case 'draw:g':
                $style = $odfNode->getAttribute('draw:style-name');
                /*if (isset($this->_style['img.' .$style])) {
                    $this->_style['div.' .$style] = 
                            $this->_style['img.' .$style];
                }*/
                
                break;
            case 'draw:custom-shape':
                $style = $odfNode->getAttribute('draw:style-name');
                $pStyle = 'position: relative; z-index: 100;';
                $this->_style['div.' .$style . ' p'] = $pStyle;
                $this->_style['div.' .$style . ' ul'] = $pStyle;
                if (isset($this->_style['div.' .$style])) {
                    //if there was a textarea-horizontal-align different
                    //from left we remove left paddings
                    $rpl = \strpos($this->_style['div.' .$style], 'rpl;');
                    if ($rpl) {
                        $regex = '/padding-left:[\s0-9a-z\.]*!important;/';
                        $this->_style['div.' .$style] = \preg_replace($regex,
                                            '', $this->_style['div.' .$style]);
                        $this->_style['div.' .$style] = \str_replace('rpl;',
                                            '', $this->_style['div.' .$style]);
                    }
                    //if there was a textarea-vertical-align different
                    //from top we remove top paddings ansd relocate childs
                    $regex = '/vertical-align:\s*([a-z]+)\s*!important;/';
                    $exp = $this->_style['div.' .$style];
                    preg_match($regex, $exp, $matches);
                    if (\count($matches) > 1) {
                        $va = $matches[1];
                        if ($va == 'middle') {
                            $css = 'top: 50%; transform: translateY(-50%)';
                            $this->_style['div.' .$style . ' p'] .= $css;
                            $regex = '/padding-top:\s*([a-z0-9\.]+)';
                            $regex .= '\s*!important;/';
                            $exp = \preg_replace($regex, '', $exp);
                        } else if ($va == 'bottom') {
                            //we use the same css because if not muiltiline
                            //content may move out its containing box
                            $css = 'top: 50%; transform: translateY(-50%)';
                            $this->_style['div.' .$style . ' p'] .= $css;
                            $regex = '/padding-top:\s*([a-z0-9\.]+)';
                            $regex .= '\s*!important;/';
                            $exp = \preg_replace($regex, '', $exp);
                        }                       
                    }
                    
                    $this->_style['div.' .$style] = $exp;
                    //we need now to rescale the stroke width by a factor
                    //in order to show it properly in a browser
                    $regex = '/stroke-width:\s*([0-9\.]*)([a-z]*)/';
                    preg_match($regex, $exp, $matches);
                    if (count($matches) > 2 
                        && isset(self::$strokeScales[$style])) {
                        $newDim = $matches[1] * self::$strokeScales[$style] 
                                  * 1.5;
                        $replace = 'stroke-width: ' . $newDim . $matches[2];
                        $exp = str_replace($matches[0], $replace, $exp);
                    }
                    $this->_style['svg.' .$style] = $exp;
                    
                }
                break;
                
        }
    }
    
    /**
     * This method recursively translate ODF nodes into HTML5 nodes
     *
     * @param DOMNode $odfNode
     * @param DOMNode $htmlNode
     * @param bool $append
     * @return mixed
     * @access private
     */
    private function _parseODFNode($odfNode, $htmlNode, $append = true)
    { 
        $stopOdfNodes = array('text:note-body' => true,
                              'text:note-citation' => true,
                              'text:table-of-content-source' => true,
                              'form:text' => true,
                              'form:textarea' => true,
                              'form:password' => true,
                              'form:file' => true,
                              'form:formated-text' => true,
                              'form:number' => true,
                              'form:date' => true,
                              'form:time' => true,
                              'form:fixed-text' => true,
                              'form:combobox' => true,
                              'form:item' => true,
                              'form:listbox' => true,
                              'form:option' => true,
                              'form:button' => true,
                              'form:image' => true,
                              'form:checkbox' => true,
                              'form:radio' => true,
                              'form:frame' => true,
                              'form:image-frame' => true,
                              'form:hidden' => true,
                              'form:grid' => true,
                              'form:column' => true,
                              'form:value-range' => true,
                              'form:generic-control' => true,
                              'svg:desc' => true,
                              'svg:title' => true,
                              /*'draw:g' => true*/);
        //TODO: check all stop tags and take care of possible  body childs that 
        //are not to be parsed
        $odfTag = $odfNode->nodeName;
        //create and insert the node
        $newNode = $this->_createHTMLNode($odfNode, $htmlNode, $append);
        if ($odfNode->hasChildNodes() 
            && !isset($stopOdfNodes[$odfTag])
            && $newNode !== NULL){
            $childs = $odfNode->childNodes;
            foreach ($childs as $child) {
                //we set $append to true to make sure that childs are inserted
                //in the node
                $this->_parseODFNode($child, $newNode, true);
            }
        }
        if ($append === false 
            && $newNode !== NULL
            && $newNode->nodeName != 'body') {
            return $newNode;
        }
    }
    
    /**
     * This method parses the attributes of the style nodes
     *
     * @param DOMNode $node
     * @param string $style
     * @param bool $important
     * @return string
     * @access private
     */
    private function _parseODFProps($node, $style = '', $important = false)
    {
        $parsedChilds = array('style:background-image' => true);
        if ($node->hasAttributes()) {
            foreach ($node->attributes as $attr) {
                $name = $attr->nodeName;
                $temp = \explode(':', $name);
                $namespace = \array_shift($temp);
                $arrayName = $namespace . '_' . 'attributes';
                if (isset(Resources::$$arrayName)) {
                    $prop = Resources::$$arrayName;
                }
                if (isset($prop[$name]) && \is_string($prop[$name])) {
                    $style .= $this->_filterProp($prop[$name],
                                                 $attr->nodeValue,
                                                 $important);
                } else if (isset($prop[$name]) && \is_array($prop[$name])) {
                    if (isset($prop[$name]['name'])
                        && isset($prop[$name][$attr->nodeValue])) {
                       $style .= $prop[$name]['name'] . ': ' . 
                                 $prop[$name][$attr->nodeValue] ;
                       if ($important) {
                            $style .= ' !important;';
                        } else {
                            $style .= ';';
                        }
                    } else if (isset($prop[$name]['method'])) {                        
                        $style .= $this->$prop[$name]['method']($name,
                                                    $attr->nodeValue); 
                    } else if (isset($prop[$name]['composedProp'])) {                        
                        $style .= $this->$prop[$name]['composedProp']($node); 
                    }
                }
            }
        }
        if ($node->hasChildNodes()) {
            $childs = $node->childNodes;
            foreach ($childs as $child) {
               if (isset($parsedChilds[$child->nodeName])) {
                   //TODO: take care of the path to the image that now
                   //will be broken
                   $style = $this->_parseODFProps($child, $style);
               } 
            }
        }
        if ($node->nodeName == 'style:paragraph-properties') {
            //In the case of paragraph properties one should remove paddings
            //for borderless pargraphs because they are not rendered by
            //Word, Libre/Open Office
            $style = $this->_clearPaddings($style);
        }

        return $style;
    }
    
    /**
     * This method parses the attributes of the style nodes
     *
     * @param DOMNode $styleNode
     * @return void
     * @access private
     */
    private function _parseODFStyle($styleNode)
    {
        $type = $styleNode->nodeName;
        if ($type == 'style:master-page') {
            $this->_parseODFStyleMaster($styleNode);
        } else if ($type == 'style:page-layout') {
            $this->_parseODFStyleLayout($styleNode);
        }  else if ($type == 'text:list-style') {
            $this->_parseODFStyleList($styleNode);
        } else {
            $this->_parseODFStyleGeneric($styleNode);
        }
        
    }
    
    /**
     * This method parses the list style nodes and their attributes
     *
     * @param DOMNode $styleNode
     * @return void
     * @access private
     */
    private function _parseODFStyleGeneric($styleNode)
    {
        //TODO: this has to be redone because if they are no child to the style
        //the inheritance is broken!!!!
        $type = $styleNode->nodeName;
        $family = $styleNode->getAttribute('style:family');
        if (isset(Resources::$style_family[$family])) {
            $tag = Resources::$style_family[$family];
        } else {
            $tag = 'div';
        }
        
        $class = $styleNode->getAttribute('style:name');
        if (!empty($class)) {
            $class = $this->_rep($class);
            $base = $tag . '.' . $class;
        } else {
            $base = $tag;            
        }
        $this->_style[$base] = '';
        //look for parent styles
        $parent  = $styleNode->getAttribute('style:parent-style-name');
        $list = $styleNode->getAttribute('style:list-style-name');
        //store the parent child relationship
        if (!empty($parent)) {
            $this->_parentStyle[$base] = $tag . '.' . $parent;
            $this->_style2style[$class] = $parent;
        }
        
        //let us now extract the attributes from the different childs
        $childs = $styleNode->childNodes;
        foreach ($childs as $child) {
            $prop = $child->nodeName;
            if (isset(Resources::$style_childs[$prop])
                && !isset(Resources::$stop_parsing_child[$type][$prop])){
                if(Resources::$style_childs[$prop] != $tag) {
                    $style = $base . ' ' . Resources::$style_childs[$prop];
                    $pStyle = Resources::$style_childs[$prop];
                } else {
                    $style = $base;
                    $pStyle = '';
                }
                if ($pStyle != '' || empty($class)) {
                    $this->_style[$style] = $this->_parseODFProps($child);
                } else {
                    //this has to be important props because if not the higher
                    //CSS specificity of other styles will overwrite this
                    //styles
                    $this->_style[$style] = $this->_parseODFProps($child, 
                                                                  '',
                                                                  true);
                }
                if (!empty($parent) /*&& empty($list)*/) {                   
                    //we need to map all possible styles to the parent
                    //even if they are not defined to assure proper inheritance
                    $this->_parentStyle[$base] = $tag . '.' . $parent;
                    $subStyles = array('canvas',
                                       /*'img',*/
                                       'p',
                                       'div',
                                       'table',
                                       'td',
                                       'col',
                                       'tr',
                                       'span');
                    foreach ($subStyles as $value) {
                        if ($value != $tag) {
                            $key = $base . ' ' . $value;
                            $parentStyle = $tag . '.' . $parent . ' ' . $value;
                            $this->_parentStyle[$key] = $parentStyle;
                        }
                    }
                }
            }            
        }  
        /*if (!empty($list) && $tag == 'p') {
            $this->_style[$base] .= 'display: table-cell;';
        }*/
    }
    
     /**
     * This method parses the attributes of generic/default style nodes
     *
     * @param DOMNode $styleNode
     * @return void
     * @access private
     */
    private function _parseODFStyleList($styleNode)
    {
        $tag = 'ul';
        $class = $styleNode->getAttribute('style:name');
        if (!empty($class)) {
            $class = $this->_rep($class);
            $base = $tag . '.' . $class;
        } else {
            $base = $tag;            
        }
        
        //TODO: take into account the attr: text:consecutive-numbering
        //<text:list-level-style-bullet>, <text:list-level-style-image> and <text:list-level-style-number>

        //let us now extract the attributes from the different childs
        $childs = $styleNode->childNodes;
        foreach ($childs as $child) {
            $listType = $child->nodeName;
            if ($listType == 'text:list-level-style-bullet') {
                $this->_parseODFStyleListBullet($child, $base);
            } else if ($listType == 'text:list-level-style-number') {
                $this->_parseODFStyleListNumber($child, $base);
            } else if ($listType == 'text:list-level-style-image') {
                $this->_parseODFStyleListImage($child, $base);
            }
        }  
    }
    
    /**
     * This method parses the list style attributes for bulleted lists
     *
     * @param DOMNode $node
     * @param string $base
     * @return void
     * @access private
     */
    private function _parseODFStyleListBullet($node, $base)
    {
        //get the list level
        $level = $node->getAttribute('text:level');
        //get the char used as bullet (UTF-8 encoded)
        $content = $node->getAttribute('text:bullet-char');
        //The text style used for the bullet
        $parentStyle = 'span.' . $node->getAttribute('text:style-name');
        //in order to accomodate arbitrary bullets we are going to
        //use the li:before pseudoclass so we have to define no list-style-type
        //at the ul level
        $baseUL = $base;
        $baseLI = $base . ' li';
        if ($level == 1) {
            $this->_style[$baseUL] = 'margin-left: 0;';
            $this->_style[$baseUL] .= 'padding: 0; list-style-type: none;';
        }
        $baseLIBefore = $base . ' li:before';
        for ($j = 1; $j < $level; $j++) {
            $baseLI .= ' ul li';
        }
        for ($j = 1; $j <= $level; $j++) {
            $baseUL .= ' ul';
        }
        $baseLIBefore = $baseLI . ':before';
        if (!empty($content)) {
            $content = \json_encode($content);
            $content = \str_replace('u' , '', $content);
            if ($content == '"\f0a7"') {
                $content = '"\25aa"';
                $style = 'content: ' . $content . ';';
            } else if ($content == '"\f0b7"') {
                $content = '"\2022"';
                $style = 'content: ' . $content . ';';
                $style .= 'font-family: Verdana !important;';
            } else {
                $style = 'content: ' . $content . ';';
            }
        } else {
            $style = '';
        }
        
        //now extract the styling info for the list items
        $childs = $node->childNodes;
        foreach ($childs as $child) {        
            $name = $child->nodeName;
            if ($name == 'style:list-level-properties'
                && $child->hasChildNodes()) {
                //TODO: check differences between values of the attribute
                //text:list-level-position-and-space-mode
                $mL = $child->firstChild->getAttribute('fo:margin-left');
                //we have to remove the margin-left because in CSS the margins
                //of nested li elements add up and this is not the standard
                //ODF behaviour
                if (!empty($mL)){
                    $ulStyle = 'margin-left: -' . $mL . ';';
                } else {
                    $ulStyle = 'margin-left: 0;';
                }
                $ulStyle .= 'padding: 0; list-style-type: none;';
                $this->_style[$baseUL] = $ulStyle;
                //build the li style
                $this->_style[$baseLI] = '';
                if (!empty($mL)) {
                    $this->_style[$baseLI] .= 'margin-left: ' . $mL . ';';
                }
                $tI = $child->firstChild->getAttribute('fo:text-indent');
                if (!empty($tI)) {
                    $this->_style[$baseLI] .= 'text-indent: ' .$tI . ';';
                    $style .= 'margin-left: ' .$tI . ';';
                    //$pR = \str_replace('-', '', $tI);
                    //$style .= 'padding-right: ' . $pR. '; ';
                }
                $style .= 'display: table-cell;';            
            }
            $this->_style[$baseLIBefore] = $style;
            if (!empty($parentStyle)) {
                $this->_parentStyle[$baseLIBefore] = $parentStyle;
            }
        }
        
    }
    
    /**
     * This method parses the list style attributes for lists with
     * "image bullets"
     *
     * @param DOMNode $styleNode
     * @param string $base
     * @return void
     * @access private
     */
    private function _parseODFStyleListImage($node, $base)
    {
    //get the list level
        $level = $node->getAttribute('text:level');
        //get the image source path
        $src = $node->getAttribute('xlink:href');
        //The text style used for the bullet
        $parentStyle = 'span.' . $node->getAttribute('text:style-name');
        //in order to accomodate arbitrary bullets we are going to
        //use the li:before pseudoclass so we have to define no list-style-type
        //at the ul level
        $baseUL = $base;
        $baseLI = $base . ' li';
        if ($level == 1) {
            $this->_style[$baseUL] = 'margin-left: 0;';
            $this->_style[$baseUL] .= 'padding: 0; list-style-type: none;';
        }
        $baseLIBefore = $base . ' li:before';
        for ($j = 1; $j < $level; $j++) {
            $baseLI .= ' ul li';
        }
        for ($j = 1; $j <= $level; $j++) {
            $baseUL .= ' ul';
        }
        $baseLIBefore = $baseLI . ':before';
        $style = 'content: " ";';
        if (!empty($src)) {
            $bg = 'url(' . $this->_imagePath($src, true) . ')';
            $style .= 'background-image: ' . $bg . ';';
            $style .= 'background-size: contain;';
            $style .= 'background-repeat: no-repeat;';
        }         
        
        //now extract the styling info for the list items
        $childs = $node->childNodes;
        foreach ($childs as $child) {        
            $name = $child->nodeName;
            if ($name == 'style:list-level-properties'
                && $child->hasChildNodes()) {
                //TODO: check differences between values of the attribute
                //text:list-level-position-and-space-mode
                //get the required attributes
                $height = $child->getAttribute('fo:height');
                if (!empty($height)) {
                    $style .= 'height: ' . $height . ';';
                }
                $width = $child->getAttribute('fo:width');
                if (!empty($height)) {
                    $style .= 'width: ' . $width . ';';
                }
                $padding = $child->getAttribute('text:space-before');
                if (!empty($padding)) {
                    $style .= 'padding-right: ' . $padding . ';';
                }
                $mL = $child->firstChild->getAttribute('fo:margin-left');
                //we have to remove the margin-left because in CSS the margins
                //of nested li elements add up and this is not the standard
                //ODF behaviour
                if (!empty($mL)){
                    $ulStyle = 'margin-left: -' . $mL . ';';
                } else {
                    $ulStyle = 'margin-left: 0;';
                }
                $ulStyle .= 'padding: 0; list-style-type: none;';
                $this->_style[$baseUL] = $ulStyle;
                //build the li style
                $this->_style[$baseLI] = '';
                if (!empty($mL)) {
                    $this->_style[$baseLI] .= 'margin-left: ' . $mL . ';';
                }
                $tI = $child->firstChild->getAttribute('fo:text-indent');
                if (!empty($tI)) {
                    $this->_style[$baseLI] .= 'text-indent: ' .$tI . ';';
                    $style .= 'margin-left: ' .$tI . ';';
                    //$pR = \str_replace('-', '', $tI);
                    //$style .= 'padding-right: ' . $pR. '; ';
                }
                $style .= 'display: table-cell;';            
            }
            $this->_style[$baseLIBefore] = $style;
            if (!empty($parentStyle)) {
                $this->_parentStyle[$baseLIBefore] = $parentStyle;
            }
        }
    }
    
    /**
     * This method parses the list style attributes for numbererd lists
     *
     * @param DOMNode $styleNode
     * @param string $base
     * @return void
     * @access private
     */
    private function _parseODFStyleListNumber($node, $base)
    {
        //get the list level
        $level = $node->getAttribute('text:level');
        //get the number format
        $numFormat = $node->getAttribute('style:num-format');
        //list-style-type equivalences
        $translate = array( '1' => 'decimal',
                            'a' => 'lower-latin',
                            'A' => 'upper-latin',
                            'i' => 'lower-roman',
                            'I' => 'upper-roman',
                            );
        if (!empty($numFormat) && isset($translate[$numFormat])) {
            $listStyleType = $translate[$numFormat];
        } else {
            $listStyleType = 'decimal';
        }
        $suffix = $node->getAttribute('style:num-suffix');
        if (empty($suffix)) {
            $suffix = '.';
        }
        $prefix = $node->getAttribute('style:num-prefix');
        $displayLevels = $node->getAttribute('text:display-levels');
        if (empty($displayLevels)) {
            $displayLevels = 1;
        }
        $textStyle = $node->getAttribute('text:style-name');
        if (empty($textStyle)) {
            $style = '';
        } else if (isset($this->_style['span.' . $textStyle])) {
            $style = $this->_style['span.' . $textStyle];
        } else {
            $style = '';
        }
        //in order to accomodate arbitrary numbering we are going to
        //use the li:before pseudoclass so we have to define no list-style-type
        //at the ul level
        $baseUL = $base;
        $baseLI = $base . ' li';
        if ($level == 1) {
            $this->_style[$baseUL] = 'counter-reset: level_1;';
            $this->_style[$baseUL] .= 'margin-left: 0;';
            $this->_style[$baseUL] .= 'padding: 0; list-style-type: none;';
        }
        $baseLIBefore = $base . ' li:before';
        for ($j = 1; $j < $level; $j++) {
            $baseLI .= ' ul li';
        }
        for ($j = 1; $j <= $level; $j++) {
            $baseUL .= ' ul';
        }
        $baseLIBefore = $baseLI . ':before';
        //set the style counters
        $style .= 'counter-increment: level_' . $level . ';';
        if (!empty($prefix)) {
            $style .= 'content: "' . $prefix . '" ';    
        } else {
            $style .= 'content: ';
        }
        $j_0 = $level - $displayLevels + 1;
        for ($j = $j_0; $j <= $level; $j++) {
            $style .= 'counter(level_' . $j . ', ' . $listStyleType. ') ';
            if ($j == $level && !empty($prefix) && $prefix != ' ') {
                $style .= ' "' .$suffix . '\00a0 ";';
            } else if ($j == $level) {
                $style .= ' "' .$suffix . ' ";';
            } else {
                $style .= ' "' .$suffix . '" ';
            }
        }
        
        //now extract the styling info for the list items
        $childs = $node->childNodes;
        foreach ($childs as $child) { 
            $name = $child->nodeName;
            if ($name == 'style:list-level-properties'
                && $child->hasChildNodes()) {
                //TODO: check differences between values of the attribute
                //text:list-level-position-and-space-mode
                $mL = $child->firstChild->getAttribute('fo:margin-left');
                //we have to remove the margin-left because in CSS the margins
                //of nested li elements add up and this is not the standard
                //ODF behaviour
                if (!empty($mL)){
                    $ulStyle = 'margin-left: -' . $mL . ';';
                } else {
                    $ulStyle = 'margin-left: 0;';
                }
                $ulStyle .= 'padding: 0; list-style-type: none;';
                $this->_style[$baseUL] = 'counter-reset: level_';
                $this->_style[$baseUL] .= ($level + 1) . ';';
                $this->_style[$baseUL] .= $ulStyle;
                //build the li style
                $this->_style[$baseLI] = '';
                if (!empty($mL)) {
                    $this->_style[$baseLI] .= 'margin-left: ' . $mL . ';';
                }
                $tI = $child->firstChild->getAttribute('fo:text-indent');
                if (!empty($tI)) {
                    $this->_style[$baseLI] .= 'text-indent: ' .$tI . ';';
                    $style .= 'margin-left: ' .$tI . ';';
                    //$pR = \str_replace('-', '', $tI);
                    //$style .= 'padding-right: ' . $pR. '; ';
                }
                $style .= 'display: table-cell;';            
            }
            $this->_style[$baseLIBefore] = $style;
            if (!empty($parentStyle)) {
                $this->_parentStyle[$baseLIBefore] = $parentStyle;
            }
        }
    }
    
    /**
     * This method parses the layout style nodes
     *
     * @param DOMNode $styleNode
     * @return void
     * @access private
     */
    private function _parseODFStyleLayout($styleNode)
    {
        //this styles are associated with different page layouts
        $name= $styleNode->getAttribute('style:name');
        $childs = $styleNode->childNodes;
        foreach ($childs as $child) {
            $sType = $child->nodeName;
            switch ($sType) {
                case 'style:page-layout-properties':
                    $this->_pageStyle[$name]['page'] = 
                        $this->_parseODFProps($child);
                    //check for childs that are not just a background image 
                    //taht is already taken care by _parseODFProps
                    $columns = $child->getElementsByTagName('columns');
                    if ($columns->length > 0) {
                        $this->_pageStyle[$name]['page'] .= 
                            $this->_parseODFProps($columns->item(0));
                    }
                    $sep = $child->getElementsByTagName('column-sep');
                    if ($sep->length > 0) {
                        $rule = $this->_parseLine($sep->item(0));
                        $this->_pageStyle[$name]['page'] .= 
                            'column-rule: ' . $rule . ';';
                    }
                    break;
                case 'style:header-style':
                case 'style:footer-style':
                    $type = \substr($sType, 6, 6);
                    $properties = $child
                        ->getElementsByTagName('header-footer-properties');
                    if ($properties->length > 0) {
                        $this->_pageStyle[$name][$type] = 
                            $this->_parseODFProps($properties->item(0));
                    }
                    break;
            }
        }
        
        //var_dump($this->_pageStyle);
        /*
         <style:page-layout style:name="PL0">
            <style:page-layout-properties style:writing-mode="lr-tb" fo:margin-left="0.7875in" fo:margin-bottom="0.7875in" fo:margin-top="0.7875in" style:num-format="1" fo:margin-right="0.7875in" style:print-orientation="portrait" fo:page-height="11.693in" fo:page-width="8.268in">
                <style:footnote-sep style:adjustment="left" style:line-style="solid" style:color="#000000" style:rel-width="33%" style:width="0.007in"/>
            </style:page-layout-properties>

        </style:page-layout>*/
    }
    
    /**
     * This method parses the master style nodes
     *
     * @param DOMNode $styleNode
     * @return void
     * @access private
     */
    private function _parseODFStyleMaster($styleNode)
    {
        
        //this styles are associated with different page Layouts
        $master= $styleNode->getAttribute('style:name');
        $layout = $styleNode->getAttribute('style:page-layout-name');
        $this->_masterLayout[$master]['layout'] = $layout;
        //we need to know which automatic styles are associated with
        //this master style
        $xpath = $this->_xpath;
        $query = '//style:style[@style:master-page-name="' . $master .'"]';
        $sNodes = $xpath->query($query);
        foreach ($sNodes as $sNode) {
            $sName = $sNode->getAttribute('style:name');
            $this->_style2master[$sName] = $master;
        }
        //but unfortunately this is not all because we have to take into account
        //styles in the styles.xml file
        $xpath = new \DOMXPath($this->_dom['styles.xml']);
        $query = '//style:style[@style:master-page-name="' . $master .'"]';
        $sNodes = $xpath->query($query);
        foreach ($sNodes as $sNode) {
            $sName = $sNode->getAttribute('style:name');
            $this->_style2master[$sName] = $master;
        }
        //the master style may be a (paragraph) style by itself so we have
        //to add it to the list
        $this->_style2master[$master] = $master;
        //TODO: take into account the next-style-name attribute!!!
        //although the problem is that it is only used when the page is
        //filled and we can not control that :-(

        $this->_parseHeaderFooter($styleNode, $master, 'header');
        $this->_parseHeaderFooter($styleNode, $master, 'footer');
        
        //footers
    }
    
    /**
     * Parses the opacity properties
     *
     * @param string $name
     * @param string $value
     * @return string
     * @access private
     */
    private function _parseOpacity($name, $value)
    {   
        $val = \trim(\str_replace('%', '', $value));
        $val = $val/100;
        if ($name == 'svg:stroke-opacity') {
            return 'stroke-opacity: ' . $val . ';';
        } else if ($name == 'draw:opacity') {
            return 'fill-opacity: ' . $val . ';';
        }
    }
    
    /**
     * Parses the background image path
     *
     * @param string $src
     * @return string
     * @access private
     */
    private function _parseSVGBackgroundImage($name, $value)
    {   
        if (isset($this->_backImages[$value])) {
            $bg = $this->_imagePath($this->_backImages[$value], true);
            return 'background-image: url(' . $bg . ');';
        }
    }
    
    /**
     * Leaves a mark to later remove left padding for elements that are 
     * horizontally aligned in SVG
     *
     * @param string $src
     * @return string
     * @access private
     */
    private function _parseTAH($name, $value)
    {   
        if ($value != 'left') {
            return 'rpl;';
        }
    }
    
    /**
     * takes acre of wrapping in frames
     *
     * @param DOMNode $node
     * @return void
     * @access private
     */
    private function _parseWrap($node)
    {
        $wrap = $node->getAttribute('style:wrap');
        
        if ($wrap == 'none') {
            return 'float: none;';
        } else if (!empty($wrap)) {
            $style = 'float: left;';
            $side = $node->getAttribute('style:horizontal-pos');
            if ($side == 'right') {
                $style = 'float: right;';
            } else if ($side == 'center') {
                $style = 'float: none; display: block;';
                $style .= 'margin-left: auto; margin-right: auto;';
            }
            return $style;
        } 
    }
    
    /**
     * This method renders the HTML
     *
     * @param array $options
     * @return void
     * @access public
     */
    public function renderHTML($options = array())
    {     
        // CSS   
        //Take into account parent styles inheritance
        $this->_mergeParentStyles();    
        //include default page styles;
        $this->_CSS .= $this->_externalCSS .PHP_EOL;
        if ($this->_parseLayout) {
            foreach ($this->_pageStyle as $key => $value) {
                //we should replace the margins by paddings for proper browser
                //rendering
                $css = $value['page'];
                $this->_CSS .= 'div#' . $this->_prefix . 'div.' . $key . ' {' . $css . '}' . PHP_EOL; 
            }
            $width = self::extractSingleProperty('data-width', $css);
            if (!empty($width)) {
                $w = 'width: ' . $width .';';
                $this->_CSS .= '.h5p_page_'. $key .' {' . $w . '}' .PHP_EOL;
            } 
        }

        foreach ($this->_style as $key => $value) {
            $posFontSize = \strpos($value, 'font-size:');
            if ($posFontSize === false && isset($this->_defaultFontSize)) {
                $value .= 'font-size: ' . $this->_defaultFontSize . ';';
            }
            $this->_CSS .= ' div#' . $this->_prefix  . $key . ' {' . $value . '}' . PHP_EOL; 
        }
        $html = '';

        if ($this->_singleFile) {
            $html .= '<style>' . PHP_EOL . $this->_CSS . PHP_EOL . '</style>';
        } else {
            if (\file_exists($this->_path['filename'] . '-styles') === false) {
                //create the folder
                \mkdir($this->_path['filename'] . '-styles');
            }
            //copy there the css data
            $path = $this->_path['filename'] . '-styles/style.css';
            $fp = \fopen($path, 'w');
            \fwrite($fp, $this->_CSS);
            \fclose($fp);
            //create the link to the CSS file
            $html .= '<link href="' . $path . '" rel="stylesheet"/>' . PHP_EOL;
        }       
        
        //HTML
        
        //start to generate the HTML5 body element
        $k = 0;
        $currentMaster = NULL;
        $layout =& $this->_masterLayout;
        foreach ($this->_HTML as $key => $value) {
            //get the master style
            if (isset($value['master'])) {
                $master = $value['master'];    
            } else {
                $master = $currentMaster;
            }
            if ($master !== $currentMaster) {
                if ($k > 0) {
                    if (!empty($layout[$master]['footer'])) {
                        $div->appendChild($layout[$master]['footer']);
                    }
                    $this->_body->documentElement->appendChild($section);
                }
                $section = $this->_body->createElement('section');
                $section->setAttribute('id', 'section_' . $k);
                if (isset($layout[$master]['layout'])) {
                    $class = 'h5p_page h5p_page_' . $layout[$master]['layout'];
                } else if (isset($layout['default'])) {
                    $class = 'h5p_page h5p_page_' . $layout['default'];
                } else {
                    $class = 'h5p_page';
                }
                $section->setAttribute('class', $class);
                $div = $this->_body->createElement('div');
                if (isset($layout[$master]['layout'])) {
                    $class = $layout[$master]['layout'];
                    $div->setAttribute('class', $class);
                } else {
                    //we use some default styles
                    $defStyle = 'border: none; padding: 2cm';
                    $div->setAttribute('style', $defStyle);
                }
                
                $section->appendChild($div);
                if (!empty($layout[$master]['header'])) {
                    $div->appendChild($layout[$master]['header']);
                }
                $currentMaster = $master;
            }  
            $div->appendChild($value['xml']);
            $k++;
        }
        //take into account the last section that is not properly handle in the
        //loop
        if (\count($this->_footnotes)  > 0 
            || \count($this->_endnotes) > 0) {
                $this->_appendNotes($div);
            }
        if (!empty($layout[$master]['footer'])) {
            $div->appendChild($layout[$master]['footer']);
        }
        $this->_body->documentElement->appendChild($section);
        //take now into account the scripts
        if (\count($this->_charts)){
            //TODO: load the javascript and CSS in a proper way :-)
            $addScripts = '<style>';
			$c3CSS = \file_get_contents(dirname(__FILE__) . ' /../../lib/vendor/js/c3.css');
			$c3CSS = \str_replace('.h5p_page', 'div#' . $this->_prefix . ' .h5p_page', $c3CSS);
            $addScripts .= $c3CSS;
            $addScripts .= '</style>';
            $addScripts .= '<script>';
            $addScripts .= \file_get_contents(dirname(__FILE__) . ' /../../lib/vendor/js/c3.js');
            $addScripts .= '</script>';
            $addScripts .= '<script>';
            $addScripts .= \file_get_contents(dirname(__FILE__) . ' /../../lib/vendor/js/d3.js');
            $addScripts .= '</script>';
            foreach ($this->_charts as $chart) {
                $addScripts .= $chart;
            }
        }
        
        //we should now repair the DOM for div's inside p's that are
        //generated for charts and images
        $this->_repairDOM($this->_body);
        $this->_body->formatOutput = false;//to avoid unwanted whitespaces
        $htmlBody = $this->_body->saveXML($this->_body->documentElement);

        if (\count($this->_charts)){
            $htmlBody = \str_replace('</body>', 
                                     $addScripts . '</body>', 
                                     $htmlBody);
        } 
        $html .= $htmlBody;

        $html = \str_replace('<body>', 
                             '<div id="' . \trim($this->_prefix) . '">',
                             $html);
        $html = \str_replace('</body>', 
                             '</div>',
                             $html);
        
        if ($this->_download) {
            return $html;
        } 
  
    }
    
    /**
     * Removes padding from borderless paragraphs
     *
     * @param string $style
     * @return string
     * @access private
     */
    private function _removePadding($style)
    {
        $regex = '/padding[^;]+;?/i';
        $style = \preg_replace($regex, '', $style);
        return \trim($style);
    }
    
    /**
     * This method removes dots from style names
     *
     * @param string $name
     * @return string
     * @access private
     */
    private function _rep($name)
    {
        $name = \str_replace('.', '_', $name);
        return $name;
    }
    
    /**
     * This method repairs the DOM
     *
     * @param DOMDocument $name
     * @access private
     */
    private function _repairDOM($dom)
    {
        $xpath = new \DOMXPath($dom);
        $query = '//p[./div]';
        $ps = $xpath->query($query);
        foreach ($ps as $p) {
            $openP = false;
            $childs = $p->childNodes;
            foreach ($childs as $child) {
                if ($child->nodeName == 'div') {
                    $openP = false;
                    $newDiv = $child->cloneNode(true);
                    $p->parentNode->insertBefore($newDiv, $p);
                } else {
                    if ($openP === false){
                        $newP = $dom->createElement('p');
                        $class = $p->getAttribute('class');
                        $newP->setAttribute('class', $class);
                        $p->parentNode->insertBefore($newP, $p);
                    }
                    $element = $child->cloneNode(true);
                    $newP->appendChild($element);
                }
            }
            $p->parentNode->removeChild($p);
        }
    }
    
    /**
     * Gets the text-decoration attribute of the deepest span node
     *
     * @param DOMNode $node
     * @return string
     * @access private
     */
    private function _spanDecoration($node)
    {
        $decor = '';
        if ($node->hasChildNodes() 
            && $node->firstChild->nodeName == 'text:span') {
            if ($node->firstChild->hasChildNodes()
                && $node->firstChild->firstChild->nodeName == 'text:span') {
                $decor = $this->_spanDecoration($node->firstChild);
                return $decor;
            } else {
                $spanStyle = $node->firstChild
                            ->getAttribute('text:style-name');
                if (isset($this->_style['span.' . $spanStyle])) {
                    $decor = 
                    self::extractSingleProperty('text-decoration', 
                                        $this->_style['span.' . $spanStyle]);

                }
            }
        }
        return $decor;
    }
    
    /**
     * Tags HTML and ODF nodes for later correspondence
     *
     * @param DOMNode $nodeHTML
     * @param DOMNode $nodeODF
     * @return string
     * @access private
     */
    private function _tagElement($nodeHTML, $nodeODF)
    {
        $excluded = array( 'text:tab' => true,
                           'text:s' => true,
                           '#text' => true,                   
                          );
        $tag = $nodeODF->nodeName;
        if ($nodeODF->nodeType == 1 && !isset($excluded[$tag])) {
            if (isset($this->_globalCounter[$tag])) {
                $this->_globalCounter[$tag]++;   
            } else {
                $this->_globalCounter[$tag] = 1;
            }
            $nodeHTML->setAttribute('data-h5p', 
                                    $tag . '=' . $this->_globalCounter[$tag]);
            $nodeODF->setAttribute('data-h5p', 
                                   $tag . '=' . $this->_globalCounter[$tag]);
        }
    }
    
    /**
     * This method udates the style2master array taking into acount inheritance
     *
     * @param array $style2style
     * @return string
     * @access private
     */
    private function _updateStyleMasterRelationships($style2style)
    {
        foreach ($style2style as $key => $value ) {
            //run over the entries that have no parent themselves
            if (!isset($style2style[$value])) {
                if (isset($this->_style2master[$value])) {
                    $this->_style2master[$key] = $this->_style2master[$value];
                }
                unset($style2style[$key]);
            }
        }
        if (\count($style2style) > 0) {
            $this->_updateStyleMasterRelationships($style2style);
        }
    }
    
    /*Additional plugin utilities*/
    
    /**
     * returns the letter corresponding to a table row (a-z aa-az ba-bz ...)
     *
     * @param int $j
     * @return	string
     * @access	public
     * @static
     */

    public static function rowLetter($j)
    {
        $letter = '';
        $number = $j;
        while ($number > 0)
        {
            $currentLetterNumber = ($number - 1) % 26;
            $currentLetter = \chr($currentLetterNumber + 65);
            $letter = $currentLetter . $letter;
            $number = ($number - ($currentLetterNumber + 1)) / 26;
        }
        return $letter;
    }
    
    /**
     * Extracts the value of the last ocurrence of a CSS property or the one
     * that is tagged as !important if the property is given as a string,
     * otherwise the property is read from the corresponding array entry.
     *
     * @param string $prop the property we wish to extract
     * @param string $CSS the chain of CSS properties
     * @return string
     * @access public
     * @static
     */
    public static function extractSingleProperty($prop, $CSS)
    {
        $value = '';
        if (\is_string($CSS)) {
            //first check if there is an important property
            $regex = '/([^-]|^)' . $prop . '\s*:\s*([^;]*?)\s*!important/i';
            \preg_match_all($regex, $CSS, $matches);
            $results = count($matches);
            $value = \array_pop($matches[2]);
            $value = \trim($value);
            if (empty($value)) {
                //there is no !important value
                $regex = '/([^-]|^)' . $prop . '\s*:\s*([^;]*?)\s*(;|$)/i';
                \preg_match_all($regex, $CSS, $matches);
                $value = \array_pop($matches[2]);
                $value = \trim($value);
            }
        } else if (\is_array($CSS)) {
            if (isset($CSS[$prop]) && \is_string($CSS[$prop])) {
                $value = $CSS[$prop];
            } else if (isset($CSS[$prop]) && \is_array($CSS[$prop])) {
                $value = \implode('', $CSS[$prop]);
            }
        }
        
        return $value;
    }
    
    /**
     * gets a CSS property in arbitrary units and return a numeric value
     * in the requested units
     *
     * @param string $unit
     * @param string $prop
     * @return float
     * @access public
     * @static
     */
    public static function convertUnits($unit, $prop)
    {   
        if (empty($prop)) {
            return 0;
        }
        //This method does not really support relative units:
        // 1. percentages are ignored
        // 2. ems and rems are assumed to be 10pt (a reasonable estimate)
        $matches = array();
        $regex = '/([0-9]+\.?[0-9]*)\s*(px|em|rem|ex|in|cm|mm|pt|pc)?/i';
        \preg_match($regex, $prop, $matches);
        $rawVal = $matches[1];
        if (isset($matches[2])) {
            $rawUnits = $matches[2];
        } else {
            $rawUnits = 'pt';
        }
        if (!isset($rawVal)) {
            //regex failed
            return 0;
        }
        //normalize to points
        if ($rawUnits == 'pt') {
            $val = $rawVal;
        } else if ($rawUnits == 'px') {
            //px is the acronym for 'pixel' that corresponds to 0.75 points
            $val = $rawVal * 0.75;
        } else if ($rawUnits == 'cm') {
            //cm is the acronym for 'centimeter' that corresponds to 28.3465 pt
            $val = $rawVal * 28.3465;
        } else if ($rawUnits == 'mm') {
            //mm is the acronym for 'millimeter' that corresponds to 2.83465 pt
            $val = $rawVal * 283.465;
        } else if ($rawUnits == 'in') {
            //in is the acronym for 'inch' that corresponds to 72 pt
            $val = $rawVal * 72;
        } else if ($rawUnits == 'pc') {
            //pc is the acronym for 'pica' that corresponds to 12 points
            $val = $rawVal * 12;
        }  else if ($rawUnits == 'em') {
            //em: this is just an estimate
            $val = $rawVal * 10;
        } else if ($rawUnits == 'ex') {
            //ex is the acronym for 'x-height' aproximately half em
            $val = $rawVal * 5;
        } 
        //return numeric value in the requested units
        if ($unit == 'pt') {
            return $val;
        } else if ($unit == 'px') {
            //px is the acronym for 'pixel' that corresponds to 0.75 points
            return  $val/0.75;
        } else if ($unit == 'cm') {
            //cm is the acronym for 'centimeter' that corresponds to 28.3465 pt
            return  $val/28.3465;
        } else if ($unit == 'mm') {
            //mm is the acronym for 'millimeter' that corresponds to 283.465 pt
            return  $val/2.83465;
        } else if ($unit == 'in') {
            //in is the acronym for 'inch' that corresponds to 72 pt
            return  $val/72;
        } else if ($unit == 'pc') {
            //pc is the acronym for 'pica' that corresponds to 12 points
            return  $val/12;
        }  else if ($unit == 'em') {
            //em: this is just an estimate
            return  $val/10;
        } else if ($unit == 'ex') {
            //ex is the acronym for 'x-height' aproximately half em
            return  $val/5;
        } else {
            return 0;
        }
    }
    
    /**
     * generates a unique id
     *
     * @param string $prefix
     * @param int $length
     * @return string
     * @access public
     * @static
     */
    public static function generateId($prefix = 'style_')
    {
        Docxpresso\CreateDocument::$counter++;
        $prefix = Docxpresso\CreateDocument::$unid . '-' . $prefix;
        $id = $prefix . Docxpresso\CreateDocument::$counter;
        return $id;
    }
    
    /**
     * sums the given values taken into account their dimensions
     *
     * @param array $data
     * @param string $unit
     * @param boolean $number
     * @return string
     * @access public
     * @static
     */
    public static function sum($data, $unit, $number = true)
    {
        $raw = array();
        foreach ($data as $element){
            $raw[] = \floatval(self::convertUnits($unit, $element));
        }
        if ($number) {
            return array_sum($raw);
        } else {
            return array_sum($raw) . $unit;
        }
    }

}
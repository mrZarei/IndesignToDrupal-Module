<?php


function _html_to_node($html_file,$images,$settings){

    $scheme = variable_get('file_default_scheme', 'public') . '://convert-index.html';
    $source = $html_file['tmppath'];
    $destination = file_stream_wrapper_uri_normalize($scheme);
    file_unmanaged_move($source, $destination, FILE_EXISTS_REPLACE);
    $doc = new DOMDocument();
    if($wrapper=file_stream_wrapper_get_instance_by_uri("public://")){
        $path=$wrapper->realpath();
    }
    $doc->loadHTMLFile($path."\\convert-index.html");
    $xpath = new DOMXPath($doc);
    $result=array();

    foreach($settings as $field){
        $is_field_text_area=preg_match('/text_textarea/i',$field['type']);
        $is_field_atom_reference=preg_match('/atom_reference/i',$field['type']);
        $image_pattern='/^\*\*Bild:/i';
        $query=implode("' or @class='",$field['class']);
        $query = "//*[@class='" . $query . "']";
        $elements = $xpath->query($query);
        if (!is_null($elements)) {
            $field_value=array();
            foreach ($elements as $element) {
                $nodes = $element->childNodes;
                $class = $element->getAttribute('class');
                foreach ($nodes as $node) {
                    $tag = $node->nodeValue;
                    //Check if field is text area.
                    if($is_field_text_area) {
                        //Check if element is image then upload related image and wrap it in img tag.
                        if(preg_match($image_pattern,$node->nodeValue)){
                            $image_value = preg_replace($image_pattern,'',$node->nodeValue);
                            dpm($image_value);
                            $image_info=_parse_image_value($image_value);
                            dpm($image_info);
                            if(count($image_info)) {
                                $image_name = $image_info['image_name'];
                                $image_sid = _move_from_temp($images[$image_name],$image_info);
                                $atom = scald_atom_load($image_sid);
                                $render_image = _atom_image_wrapper($atom);
                                $tag = $render_image['render'];
                            }
                            else{
                                form_set_error('IDT',"Class {$class} does not have valid value!");
                            }
                        }
                        else {
                            $ck = new idtCkeditor();
                            $element_tag ="p";
                            $element_attr ="";
                            if($style=$ck->get_ckeditor_style($class)){
                                $element_tag=$style['element'];
                                foreach($style['attributes'] as $attribute=>$attValue){
                                    $element_attr .=' ' .$attribute ."='".$attValue ."'";
                                }
                            }
                            $tag = "<{$element_tag}{$element_attr}>" . $node->nodeValue . "</{$element_tag}>";
                        }
                    }
                    else if($is_field_atom_reference){
                        $image_info=_parse_image_value($node->nodeValue);
                        if(count($image_info)) {
                            $image_name = $image_info['image_name'];
                            $image_sid = _move_from_temp($images[$image_name],$image_info);
                            $tag = $image_sid;
                        }
                        else{
                            form_set_error('IDT',"Class {$class} does not have valid value!");
                        }

                    }
                }
                if(!empty($tag))
                    $field_value[] = $tag;
            }
        }
        if(isset($field_value) && count($field_value)) {
            $value = implode(' ',$field_value);
            if ($is_field_text_area) {
                $result[$field['name']]['und'][0]['value'] = $value;
                $result[$field['name']]['und'][0]['safe_value'] = $value;
                $result[$field['name']]['und'][0]['format'] = 'wysiwyg';
                if (preg_match('/_with_summary/i', $field['type'])) {
                    //remove tag by using of regex.
                    $result[$field['name']]['und'][0]['summary'] = preg_replace('/<.[^>]*>/i', '', substr($value, 0, 170));
                    $result[$field['name']]['und'][0]['safe_summary'] = preg_replace('/<.[^>]*>/i', '', substr($value, 0, 170));
                }
            }
            else if($is_field_atom_reference){
                $result[$field['name']] = array('und' => array());
                foreach ($field_value as $atom => $sid) {
                    $result[$field['name']]['und'][] = array('sid' => $sid);
                }
            }
            else if(preg_match('/^textfield$/i',$field['type'])){
                $result[$field['name']]=$value;
            }
            else{
                $result[$field['name']] = array(
                    'und'=> array(
                        array(
                            'value' => $value,
                        ),
                    ),
                );
            }
        }
    }
    if(count($result)) {
        _create_node($result);
    }
    else{
        form_set_error('itd',"No field has been set to import, please check setting of 'Indesign to Drupal module' ");
    }
}


/**
 * @param $temp_file
 *  array of temp file's info
 * @return int/bool
 *  return atom sid.
 */
function _move_from_temp($temp_file,$values){
    $scheme = variable_get('file_default_scheme', 'public') . '://';
    $source = $temp_file['tmppath'];
    $destination = file_stream_wrapper_uri_normalize($scheme . $temp_file['name']);
    // Rename it to its original name, and put it in its final home.
    // Note - not using file_move here because if we call file_get_mime
    // (in file_uri_to_object) while it has a .tmp extension, it horks.
    $destination = file_unmanaged_move($source, $destination, FILE_EXISTS_RENAME);
    $file = plupload_file_uri_to_object($destination);
    file_save($file);
    // Create an atom

    //Set Image text if it is set
    $image_text = array();
    if ($values['field_image_text']){
        $image_text = array(
            'und' => array(
                array(
                    'value' => $values['field_image_text'],
                    'safe_value' => 'safe value',
                    'format' => '',
                ),
            ),
        );
    }

    //Set Image byline
    $image_byline = array();
    if ($values['field_image_byline']){
        $image_byline = array(
            'und' => array(
                array(
                    'value' => $values['field_image_byline'],
                    'safe_value' => 'safe value',
                    'format' => '',
                ),
            ),
        );
    }

    $atom = new ScaldAtom('image','scald_image', array(
        'base_id' => $file->fid,
        'title' => $file->filename,
        'field_image_text' => $image_text,
        'field_image_byline' =>$image_byline,
    ));
    $langcode = field_language('scald_atom', $atom, 'scald_thumbnail');
    $atom->scald_thumbnail[$langcode][0] = (array) $file;
    $atom_sid=scald_atom_save($atom);
    return $atom_sid;
}

function _create_node($value)
{
    $node = new idtNode('article',$value);
    $node->saveNode();
}

function _parse_image_value($value){
    $result=array();
    $image_delimiter = "*";
    $image_info = explode($image_delimiter,$value);
    $result = array(
        'image_name' => $image_info[0],
    );
    if(isset($image_info[1]))
        $result['field_image_text'] = $image_info[1];
    if(isset($image_info[2]))
        $result['field_image_byline'] = $image_info[2];

    return $result;

}

function _atom_image_wrapper($atom){
    $return = array();
    if(isset($atom->field_image_text['und'][0]['value'])) {
        $return['items']['image_text'] ="<div class ='image-text'>" . $atom->field_image_text['und'][0]['value'] . "</div>";
    }
    if(isset($atom->field_image_byline['und'][0]['value'])) {
        $return['items']['image_byline'] ="<div class ='image-byline'>" . $atom->field_image_byline['und'][0]['value'] . "</div>";
    }

    if(isset($return['items']['image_txt']) || isset($return['items']['image_byline'])) {
        $return['items']['image_extra_wrapper'] = "<div class ='image-extra-wrapper'>";
        $return['items']['image_extra_wrapper'] .=(isset($return['items']['image_text']))? $return['items']['image_text'] : '';
        $return['items']['image_extra_wrapper'] .=(isset($return['items']['image_byline']))? $return['items']['image_byline'] : '';
        $return['items']['image_extra_wrapper'] .= "</div>";
    }
    $image_path = image_style_url('large', $atom->file_source);
    $title=(isset($atom->field_image_text['und'][0]['value']))? $atom->field_image_text['und'][0]['value'] : $atom->title;
    $return['items']['image_tag'] = "<img class='large' src='{$image_path}' title='{$title}' alt='{$title}'>" ;

    $return['render']  = '<div class="dnd-atom-wrapper type-image context-sdl_editor_representation" contenteditable="false">';
    $return['render'] .= "<div class='dnd-drop-wrapper'><div class ='image'>";
    $return['render'] .= $return['items']['image_tag'];
    $return['render'] .= (isset($return['items']['image_extra_wrapper']))? $return['items']['image_extra_wrapper'] : '';
    $return['render'] .= "</div></div>";
    $return['render'] .= "</div>";


    return $return;
}
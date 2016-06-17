<?php

class idtNode{

    private $node;

    /**
     * @param string $type
     * @param $values
     */
    public function __construct($type='article',$values=array()){

        $nodeValues=$this->setDefault($type);
        dpm($nodeValues);
        $values+=$nodeValues;
        $this->node = new stdClass();
        $this->node->type = $type;
        node_object_prepare($this->node);
        foreach ($values as $key => $value){
            $this->node->$key = $value;
        }
    }
    public  function getNode(){
        return $this->node;
    }
    private function setDefault($type='article'){
        $values =array();
        $fields = field_info_instances('node', $type);
        foreach($fields as $field) {
            $values[$field['field_name']] = array();
        }
        $values['language']="sv";
        $values['field_displaytype']['und'][0]['value'] = "style1";
        $domain_id=domain_default_id();
        $values['domains']=array($domain_id=>$domain_id);
        $values['domain_site'] = FALSE;
        $values['domain_source'] = $domain_id;
        $values['field_update_active']['und'][0]['value'] = 0;
        $values['field_article_type']['und'][0]['tid'] = 17;
        return $values;
    }
    public function saveNode(){
        node_submit($this->node);
        foreach (module_implements('node_submit') as $module) {
            $function = $module . '_node_submit';
            $function($this->node);
        }
        node_save($this->node);
        return true;
    }
}


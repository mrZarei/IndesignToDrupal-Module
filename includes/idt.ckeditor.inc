<?php

class idtCkeditor{
    private $elements;
    function __construct(){
        $this->elements = $this->initStyleSet();
    }
    private function initStyleSet(){
        $elementsList =array(
            'ingress' => array(
                'name' => 'Ingress',
                'element' => 'p',
                'attributes' =>array(
                    'class' => 'ingress'
                ),
            ),
            'fraga' => array(
                'name' => 'Fråga',
                'element' => 'p',
                'attributes' => array(
                    'class' => 'fraga',
                ),
            ),
            'mellanrubrik' => array(
                'name' => 'Mellanrubrik',
                'element' => 'h2',
                'attributes' => array(
                    'class' => 'mellanrubrik',
                ),
            ),
            'underrubrik' => array(
                'name' => 'Underrubrik',
                'element'=>'h2',
                'attributes' => array(
                    'class' => 'underrubrik'
                )
            ),
            'overrubrik' => array(
                'name' => 'Överrubrik',
                'element'=>'h2',
                'attributes' => array(
                    'class' => 'overrubrik'
                )
            ),
            'styckerubrik' => array(
                'name' => 'Styckerubrik',
                'element'=>'p',
                'attributes' => array(
                    'class' => 'styckerubrik'
                )
            ),
            'styckerubrik' => array(
                'name' => 'Styckerubrik',
                'element'=>'p',
                'attributes' => array(
                    'class' => 'styckerubrik')
            ),
        );
        return $elementsList;
    }
    public function get_ckeditor_style($className){
        if(isset($this->elements[$className]))
            return $this->elements[$className];
        return FALSE;
    }
    public function getElements(){
        return $this->elements;
    }

}
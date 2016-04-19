/**
 * Created by mohammadReza on 4/13/2016.
 */
(function($) {
    Drupal.behaviors.itd = {
        attach: function () {
            document.getElementById('itd-file').addEventListener('change', fileChange, false);
            document.getElementById('convert-btn').addEventListener('click', convert);
            function fileChange(event) {
                var file = event.target.files[0];
                if (file) {
                    var reader = new FileReader();
                    reader.onload = function (theFile) {
                        var text = theFile.target.result;
                        $("#itd-iframe").contents().find("html").html(text);

                    };
                    reader.readAsText(file);
                }
            }

            function convert() {
                var fields = Drupal.settings.itd.fields;
                console.log(fields);
                $.each(fields, function (index, value) {
                    if(value.class.length)
                        insertValue(value);
                });
            }

            function insertValue(field) {
                var content = '';
                //get file content
                var file = $("#itd-iframe").contents();

                //add "." to class array to make a proper selector
                var classes = $.map(field.class, function (item) {
                    return "." + item;
                })
                var fileElements = file.find(classes.toString());

                //We need to know the type of field to get proper element
                // and insert data into it. Drupal puts fields in a div with class like "field-name" + name of field
                var fieldName = field.name.replace(/_/ig, '-');
                var inputElement = ".field-name-" + fieldName;
                // Our code work on "Container" and "Textfield" types right now.
                if (field.type.search(/textarea/) >= 0) {
                    //In the case of container we insert content in html format
                    $.each(fileElements, function () {
                        //Define element to add to content.
                        var element=$(document.createElement('p'));
                        var fileElement=$(this);
                        //For the styles which have been defined in CKedior custom style element type should
                        //be changed, and custom class should be added.
                        if(typeof(CKEDITOR) !== 'undefined'){
                            if(typeof(CKEDITOR.stylesSet.registered.drupal)!=='undefined'){
                                //browsing in CKeditor styles
                                $.each(CKEDITOR.stylesSet.registered.drupal,function(key,value){
                                    if(typeof(value.attributes.class !== 'undefined')){
                                        if (fileElement.hasClass(value.attributes.class)) {
                                            element=$(document.createElement(value.element));
                                            element.addClass(value.attributes.class);
                                            return false;
                                        }
                                    }
                                });
                            }
                        }
                        element.html($(this).html());
                        //$(this).css("background","yellow").fadeOut(1000);
                        content += element.prop('outerHTML');
                    });
                    //In the case of container we should check if drupal uses "CKeditor" or not.
                    //If it does then we should insert data into Iframe body
                    if ($(inputElement + " iframe").length) {
                        $(inputElement + " iframe").contents().find('body').html(content);
                        fileElements.css("background","yellow").fadeOut(1000);
                    }
                    else if ($(inputElement + " textarea").length) {
                        //if CKeditor does not apply to container
                        $(inputElement + " textarea").val(content);
                        fileElements.css("background","yellow").fadeOut(1000);
                    }
                    else {
                        console.log("can not fine element: " + inputElement);
                    }
                }
                else if ((field.type.search(/textfield/) >= 0) && $("input[name^='" + field.name+"'").length) {
                    //In the case of textfield we insert value of content
                    $.each(fileElements, function () {
                        content += $(this).text();
                        $(this).css("background","yellow").fadeOut(1000);
                    });
                    //Drupal use "edit"+field name as id for textfield fields
                    //We can specify element on form by id.
                    $("input[name^='" + field.name+"'").val(content);
                }

            }
        }
    }
})(jQuery);

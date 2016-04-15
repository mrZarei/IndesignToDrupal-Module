/**
 * Created by mohammadReza on 4/13/2016.
 */
(function($) {
    Drupal.behaviors.itd = {
        attach: function () {
            document.getElementById('itd-file').addEventListener('change', fileChange, false);
            document.getElementById('convert-btn').addEventListener('click', convert);
            var fields = [
                {name: 'title', type: 'textfield', class: ['rubrik']},
                {name: 'field_preamble', type: 'container', class: ['ingress']},
                {name: 'body', type: 'container', class: ['text', 'text_indrag', 'mellanrubrik']}
            ];

            function fileChange(event) {
                var file = event.target.files[0];
                if (file) {
                    var reader = new FileReader();
                    reader.onload = function (theFile) {
                        var text = theFile.target.result;
                        $("#idt-iframe").contents().find("html").html(text);

                    };
                    reader.readAsText(file);
                }
            }

            function convert() {
                var fields = [
                    {name: 'title', type: 'textfield', class: ['rubrik']},
                    {name: 'field_preamble', type: 'container', class: ['ingress']},
                    {name: 'body', type: 'container', class: ['text', 'text_indrag', 'mellanrubrik']}
                ];
                $.each(fields, function (index, value) {
                    console.log(value);
                    insertValue(value);
                });
            }

            function insertValue(field) {
                var content = '';
                //get file content
                var file = $("#idt-iframe").contents();

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
                if (field.type == 'container') {
                    //In the case of container we insert content in html format
                    $.each(fileElements, function () {
                        content += $(this).prop('outerHTML');
                    });
                    //In the case of container we should check if drupal uses "CKeditor" or not.
                    //If it does then we should insert data into Iframe body
                    if ($(inputElement + " iframe").length) {
                        $(inputElement + " iframe").contents().find('body').html(content);
                    }
                    else if ($(inputElement + " textarea").length) {
                        //if CKeditor does not apply to container
                        $(inputElement + " textarea").val(content);
                    }
                    else {
                        console.log("can not fine element: " + inputElement);
                    }
                }
                else if (field.type == 'textfield') {
                    //In the case of textfield we insert value of content
                    $.each(fileElements, function () {
                        content += $(this).text();
                    });
                    //Drupal use "edit"+field name as id for textfield fields
                    //We can specify element on form by id.
                    $('#edit-' + fieldName).val(content);
                }

            }
        }
    }
})(jQuery);

/**
 * Created by mohammadReza on 4/13/2016.
 */
(function($) {
    Drupal.behaviors.itd = {
        attach: function () {
            document.getElementById('itd-file').addEventListener('change', fileChange, false);
            document.getElementById('convert-btn').addEventListener('click', convert);
            var field=[
                {name:'title',type:'textfield'},
                {name:'field_preamble',type:'container'},
                {name:'body',type:'container'}];
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
                var file = $("#idt-iframe").contents();
                var titleSelector = Drupal.settings.itd.titleClasses
                var bodySelector = Drupal.settings.itd.bodyClasses;
                bodySelector = bodySelector.map(function (item) {
                    return "." + item;
                });
                file.find(bodySelector.toString()).css("background", "yellow");
                titleSelector =titleSelector.map(function(item){
                    return "."+item;
                });
                var titles = file.find(titleSelector.toString());
                var title = '';
                $.each(titles, function () {
                    title += $(this).text();
                });
                console.log(title);
            }
        }
    }
})(jQuery);

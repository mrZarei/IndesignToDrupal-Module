
## Html to Drupal
# this module has been implemented intent to importing articles from Word document into drupal.




** To achieve to the proper style in  the textarea , the class of element which is going to import to textarea, should be defined
   in "idt.ckeditor class.

** All the images text which is going to place in a textarea field should be start with "**Bild:" (without space).
** The Images value can contain "Image text" and "Image Byline". In order to do this value can be split by "*", in the way that first
   phrase is image name, the second one is image text and the third is image by line
   ex: **Bild:pic1.jpg*some texts*photo by mrz =>  image name:pic1.jpg , image text: some texts, image_byline:photo by mrz
   ex  **Bild:pic1.jpg* *photo by mrz => example of value without image text.
   Note: image name is obligatory




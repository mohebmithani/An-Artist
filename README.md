# An-Artist
This is a simple photo editor program in c language. This Program only focuses on "BITMAP" images of 24 bit. It can simply copy your image and it also uses some basic filtering process like "Greyscale", "Hue Changing", "Blurring", “Saturation Changing”, “Swapping of RGB colors” etc.

# Data Structure Used:
There are five data structures used in my project.
* **BITMAPFILEHEADER**: A *struct*. It stores the basic information of an image or file like what type of file(in our case bmp), where the actual data about image will be etc. 
* **BITMAPINFOHEADER**: A *struct*. It is the actual data about the image. It is the area where manipulation or other things will be done.
* **RGBTRIPLE**: A *struct*. It is used to store RGB(Red, Green, Blue) value of each pixels.
* **HSL**: A *struct*. When we have to work on HSL(Hue, Saturation, Lightness) we have to convert RGB into HSL, thus here we store these converted values.
* **image[]**: An *array*. It is used to store image for manipulation.

# Functions Used:
Most important methods used in this program are:
* **CopyBitmapFile()** : It is used to copy the original image for editing. It reads the two section of bmp file(File and Info header). Initializes and Fills RGB struct according to the information given in headers files. If user just wants to copy image then all these information will be added to the other file otherwise *Filter()* function will be called for editing purposes. 
* **Filters()** : It will apply the filter per user demand. Following filters are supported in this program:
    1. GreyScale.
    2. Negative.
    3. Brighter Image.
    4. Darker Image.
    5. Removing Some Color Channels.
    6. Swap The Color Channels.
    7. Blur.
    8. Sharpen.
    9. Emboss.
    10. Changing Hue.
    11. Changing Saturation.
* **RGBtoHSL() / HSLtoRGB()** : It converts RGB pixel value into HSL value and vice verca.
# Some Examples:
  ## Sample Images
  ![Image of a Butterfly](/Images/Sample/Butterfly.bmp)
  ![Image of a Tiger](/Images/Sample/tiger.bmp)
  ### After Blur Effect:
  ![Blur image of a Butterfly](/Images/Edited/Butterfly_Blur.bmp)
  ![Blur image of a Tiger](/Images/Edited/tiger_Blur.bmp)
  ### After Negative Effect:
  ![Negative image of a Butterfly](/Images/Edited/Butterfly_Negative.bmp)
  ![Negative image of a Tiger](/Images/Edited/tiger_Negative.bmp)
  ### After Increasing Saturation of Butterfly:
  ![Increased Saturated image of a Butterfly](/Images/Edited/ButterflyIncreaseSat.bmp)
  ### After Decreasing Saturation of Tiger:
  ![Decreased saturated image of a Tiger](/Images/Edited/tiger_DecreaseSat.bmp)
  ### After Swapping Green and Blue color of Butterfly:
  ![Increased Saturated image of a Butterfly](/Images/Edited/ButterflySwapGB.bmp)
  
  **Other examples can be seen in /Images/Edited folder**
###### Note:
* When prompt to give name of original image and name of edited image, don't forget to write correct path with "extension" like **/Images/Sample/tiger.bmp**
* It is my first-year project so a lot of errors are expected.
* I took a lot of help from CS50 course and its image pset.

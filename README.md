# BMP Image transformer
Application for rotating bmd images. since the application is modular, it can be 
easily modified. for example, you can add support for various formats without 
touching the module with transitions. Or you can add different transitions 
(rotating clockwise, or reversing the image) without touching module that 
convert actual image to the __struct__ that represent image. So feel free to
contribute to the project  :)
## Usage 
In order to work use the project you need:

1) Clone project using __git clone https://github.com/bonchEquipment/image-rotation.git__
2) Build project by typing __make__ in the home directory
3) Execute the built binary with 2 arguments: 
* path to the image you want to transform
* path to the resulting image
```
./transformer <source-image> <resulting-image>
```

<img src="sample.gif" alt="sample"/>

## Contributing
In order to contribute to the project you need:
1) Fork this repository
2) Create new branch in forked repository
3) Make some changes
4) Do pull request

If you have any questions feel free to contact me via 
[vk](https://vk.com/mi1estone) or [telegram](https://t.me/Mi1eStone).

## License 
Image transformer is GNU licensed. 
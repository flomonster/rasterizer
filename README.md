# Rasterizer

This project is about implementing a rasterizer.

We're implementing these following algorithms:

* z-buffer
* lambert shading
* phong shading

## Setup

Requirements:

* meson
* boost
* assimp

To build the project follow these commands:

```
meson build
cd build
ninja
```

## Usage

Once you built the project you can run rasterization as followed:

```
./rasterizer --in scene.fbx --out my_picture.ppm
```

For more option use `./rasterizer --help`.

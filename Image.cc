#include "Image.h"
#include <OpenImageIO/imagebufalgo.h>
#include <iostream>

Image::Image(const std::string& path, bool squareImage):
    squareImage(squareImage),
    enframe(false),
    path(path),
    animateImage(false)
{
    future = std::async(std::launch::async, &Image::init, this, path);
}

void
Image::init(const std::string& path)
{
    buffer.reset(path);

    if (buffer.nsubimages() == 0)
        errormsg = "Not an image: " + path;
    else
    {
        animateImage = !(buffer.nsubimages() == 1);
        readPixels();
    }
}

void
Image::readPixels()
{
    if (buffer.nchannels() != 4)
        buffer = fixChannels(buffer);

    sf::Uint8 *pixels = new sf::Uint8[buffer.roi().width() * buffer.roi().height() * 4];
    bool ok = buffer.get_pixels(buffer.roi(), OIIO::TypeDesc::UINT8, pixels);
    if (!ok || buffer.has_error())
    {
        errormsg = "Error loading image: " + path;
        return;
    }

    texture.create(buffer.roi().width(), buffer.roi().height());
    texture.update(pixels);
    texture.setSmooth(true);
    sprite.setTexture(texture, true);

    delete[] pixels;
    valid = true;
    if (squareImage) square();
    if (enframe) fitTo(frame);
    if (animateImage)
    {
        float fps = buffer.spec().get_float_attribute("FramesPerSecond");
        delay = sf::seconds((fps == 0.f) ? .1f : 1.f / fps);
    }
    clock.restart();
}

OIIO::ImageBuf
Image::fixChannels(OIIO::ImageBuf& buffer)
{
    if (buffer.nchannels() == 3)
        return OIIO::ImageBufAlgo::channels(buffer, 4,
                /* channelorder */ { 0, 1, 2, -1 /*use a float value*/ },
                /* channelvalues */ { 0 /*ignore*/, 0 /*ignore*/, 0 /*ignore*/, 1.0 },
                /* channelnames */ { "", "", "", "A" });
    else if (buffer.nchannels() == 1)
        return OIIO::ImageBufAlgo::channels(buffer, 4,
                /* channelorder */ { 0, 0, 0, -1 /*use a float value*/ },
                /* channelvalues */ { 0 /*ignore*/, 0 /*ignore*/, 0 /*ignore*/, 1.0 },
                /* channelnames */ { "R", "G", "B", "A" });
    else
    {
        errormsg = path + ": Incorrect number of channels: " + std::to_string(buffer.nchannels());
        return buffer;
    }
}

void
Image::fitTo(const sf::Vector2u& window)
{
    if (valid)
    {
        const sf::Vector2u& size = sprite.getTexture()->getSize();

        float xScale = (float)window.x / size.x;
        float yScale = (float)window.y / size.y;
        float scale = (xScale < yScale) ? xScale : yScale;
        sprite.setScale(scale, scale);

        sprite.setOrigin(size.x / 2, size.y / 2);
        sprite.setPosition(window.x / 2, window.y / 2);
        enframe = false;
    }
    else
    {
        enframe = true;
        frame = window;
    }
}

void
Image::update()
{
    if (!animateImage) return;

    if (clock.getElapsedTime() > delay)
    {
        int index = buffer.subimage() + 1;
        if (index == buffer.nsubimages())
            index = 0;

        buffer.read(index);
        readPixels();

        clock.restart();

        if (squareImage) square();
    }
}

void
Image::square()
{
    if (valid)
    {
        auto size = sprite.getTexture()->getSize();
        auto offset = std::abs((int)size.x - (int)size.y) / 2;

        if (size.x < size.y)
            sprite.setTextureRect(sf::IntRect(0, offset, size.x, size.x));
        else
            sprite.setTextureRect(sf::IntRect(offset, 0, size.y, size.y));
    }
}
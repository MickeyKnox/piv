#include "ScrollView.h"
#include <iostream>
#include <set>

ScrollView::ScrollView(Folder& folder, sf::RenderWindow& window):
    folder(folder),
    window(window)
{
    firstItem = folder.currentItem;
    lastItem = folder.currentItem;
    initImages();
}

bool
ScrollView::instanceOf(const SubType& subType)
{
    return subType == SubType::ScrollView;
}

void
ScrollView::handle(sf::Event& event)
{
    switch (event.type)
    {
        case sf::Event::MouseWheelScrolled:
            if (event.mouseWheelScroll.delta < 0)
                scrollDown(1);
            else if (event.mouseWheelScroll.delta > 0)
                scrollUp(1);
            break;

        case sf::Event::KeyPressed:
            switch (event.key.code)
            {
                case sf::Keyboard::Up:
                    scrollUp(1);
                    break;
                case sf::Keyboard::Down:
                    scrollDown(1);
                    break;
                case sf::Keyboard::PageUp:
                    scrollUp(1);
                    break;
                case sf::Keyboard::PageDown:
                    scrollDown(1);
                    break;
                case sf::Keyboard::Num1:
                    numberOfColumns = 1;
                    initImages();
                    break;
                case sf::Keyboard::Num2:
                    numberOfColumns = 2;
                    initImages();
                    break;
                case sf::Keyboard::Num3:
                    numberOfColumns = 3;
                    initImages();
                    break;
                case sf::Keyboard::Num4:
                    numberOfColumns = 4;
                    initImages();
                    break;
                case sf::Keyboard::Num5:
                    numberOfColumns = 5;
                    initImages();
                    break;
                case sf::Keyboard::Num6:
                    numberOfColumns = 6;
                    initImages();
                    break;
                case sf::Keyboard::Num7:
                    numberOfColumns = 7;
                    initImages();
                    break;
                case sf::Keyboard::Num8:
                    numberOfColumns = 8;
                    initImages();
                    break;
                case sf::Keyboard::Num9:
                    numberOfColumns = 9;
                    initImages();
                    break;
                case sf::Keyboard::Num0:
                    numberOfColumns = 10;
                    initImages();
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }
}

void
ScrollView::initImages()
{
    int index = std::distance(folder.cbegin(), firstItem);
    int row =  index / numberOfColumns;
    int firstImage = row * numberOfColumns;
    int indent = index - firstImage;

    for (int i = 0; i < indent; i++)
    {
        Image* image = new Image(*--firstItem);
        images.push_front(image);
    }

    int rows = std::ceil(1.f * window.getSize().y / imageSize());
    int imageCount = rows * numberOfColumns;
    int delta = imageCount - images.size();

    if (delta > 0)
    {
        for (int i = 0; i < delta; i++)
        {
            Image* image = new Image(*lastItem++);
            images.push_back(image);

            if (lastItem == folder.cend()) break;
        }
    }
    else 
    {
        for (int i = 0; i < -delta; i++)
        {
            delete images.back();
            images.pop_back();
            lastItem--;
        }
    }

    for (auto image : images)
        image->square(imageSize());
}

void
ScrollView::scrollDown(int rows)
{
    if (lastItem == folder.cend())
    {
        int rows = std::ceil(1.f * window.getSize().y / imageSize());
        heightOffset = window.getSize().y - rows * imageSize();
    }
    else
    {
        for (int i = 0; i < numberOfColumns * rows; i++)
        {
            delete images.front();
            images.pop_front();
            firstItem++;
        }

        for (int i = 0; i < numberOfColumns * rows; i++)
        {
            Image* image = new Image(*lastItem++);
            image->square(imageSize());
            images.push_back(image);

            if (lastItem == folder.cend()) break;
        }
    }
}

void
ScrollView::scrollUp(int rows)
{
    heightOffset = 0;
    if (firstItem == folder.cbegin()) return;

    int removeCount = numberOfColumns * rows;
    if (lastItem == folder.cend())
        removeCount = folder.size() % numberOfColumns + numberOfColumns * (rows - 1);

    for (int i = 0; i < removeCount; i++)
    {
        delete images.back();
        images.pop_back();
        lastItem--;
    }

    for (int i = 0; i < numberOfColumns * rows; i++)
    {
        Image* image = new Image(*--firstItem);
        image->square(imageSize());
        images.push_front(image);

        if (firstItem == folder.cbegin()) break;
    }
}

void
ScrollView::draw()
{
    int offset = heightOffset;
    int column = 0;

    for (auto image : images)
    {
        if (image->ready)
        {
            image->update();
            image->sprite.setPosition(imageSize() * column, offset);
            window.draw(image->sprite);
        }

        column++;
        if (column == numberOfColumns)
        {
            offset += imageSize();
            column = 0;
        }
    }
}

void
ScrollView::resizeEvent()
{
    initImages();
}

void
ScrollView::selectImage()
{
    auto mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    for (auto const& image: images)
        if (image->sprite.getGlobalBounds().contains(mouse.x, mouse.y))
            folder.currentItem = std::find(folder.cbegin(), folder.cend(), image->path);
}

void
ScrollView::scrollToCurrentImage()
{
    if (folder.currentItem < firstItem || folder.currentItem >= lastItem)
    {
        for (Image* image: images)
            delete image;

        images.clear();

        firstItem = folder.currentItem;
        lastItem = folder.currentItem;
        initImages();
    }
}

int
ScrollView::imageSize()
{
    return window.getSize().x / numberOfColumns;
}

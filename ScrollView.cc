#include "ScrollView.h"
#include <iostream>

ScrollView::ScrollView(Folder& folder, sf::RenderWindow& window):
    folder(folder),
    window(window)
{
}

bool
ScrollView::instanceOf(const SubType& subType)
{
    return subType == SubType::ScrollView;
}

void
ScrollView::handle(sf::Event& event)
{
    auto view = window.getView();
    switch (event.type)
    {
        case sf::Event::MouseWheelScrolled:
            if (event.mouseWheelScroll.delta < 0)
                view.move(0, 10);
            else if (event.mouseWheelScroll.delta > 0)
                view.move(0, -10);
            window.setView(view);
            break;

        case sf::Event::KeyPressed:
            switch (event.key.code)
            {
                case sf::Keyboard::Up:
                    view.move(0, -100);
                    window.setView(view);
                    break;
                case sf::Keyboard::Down:
                    view.move(0, 100);
                    window.setView(view);
                    break;
                case sf::Keyboard::Num1:
                    numberOfColumns = 1;
                    break;
                case sf::Keyboard::Num2:
                    numberOfColumns = 2;
                    break;
                case sf::Keyboard::Num3:
                    numberOfColumns = 3;
                    break;
                case sf::Keyboard::Num4:
                    numberOfColumns = 4;
                    break;
                case sf::Keyboard::Num5:
                    numberOfColumns = 5;
                    break;
                case sf::Keyboard::Num6:
                    numberOfColumns = 6;
                    break;
                case sf::Keyboard::Num7:
                    numberOfColumns = 7;
                    break;
                case sf::Keyboard::Num8:
                    numberOfColumns = 8;
                    break;
                case sf::Keyboard::Num9:
                    numberOfColumns = 9;
                    break;
                case sf::Keyboard::Num0:
                    numberOfColumns = 10;
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
ScrollView::draw()
{
    int windowTop = window.getView().getCenter().y - window.getView().getSize().y / 2;
    int windowBot = window.getView().getCenter().y + window.getView().getSize().y / 2;
    int size = window.getView().getSize().x / numberOfColumns;
    int firstVisibleRow = windowTop / size;
    int firstImage = firstVisibleRow * numberOfColumns;
    int heightOffset = firstVisibleRow * size;
    int column = 0;
    int lastVisibleRow = std::ceil(1.f * windowBot / size);
    int lastImage = lastVisibleRow * numberOfColumns;

    auto iter = folder.cbegin();
    std::advance(iter, firstImage);

    auto end = folder.cbegin();
    std::advance(end, std::min(lastImage, (int)std::distance(folder.cbegin(), folder.cend())));

    while (iter != end)
    {
        Image* image = getImage(*iter);
        if (image->valid)
        {
            image->update();
            float factor = size / image->sprite.getLocalBounds().width;
            image->sprite.setScale(factor, factor);
            image->sprite.setPosition(size * column, heightOffset);
            window.draw(image->sprite);
        }

        iter++;
        column++;
        if (column == numberOfColumns)
        {
            heightOffset += size;
            column = 0;
        }
    }
}

Image*
ScrollView::getImage(const std::string& path)
{
    if (imageCache.count(path) == 0)
    {
        Image* image = new Image(path, true);
        imageCache.insert(std::pair<std::string, Image*>(path, image));
    }
    return imageCache.find(path)->second;
}

void
ScrollView::fullscreenToggle()
{
}

void
ScrollView::selectImage()
{
    /*
    imageCache.currentImage = imageCache.begin();

    auto mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    for (auto iter = imageCache.begin(); iter != imageCache.end(); iter++)
        if ((**iter).getSquareSprite().getGlobalBounds().contains(mouse.x, mouse.y))
            imageCache.currentImage = iter;
            */
}

void
ScrollView::scrollToCurrentImage()
{
    /*
    auto bounds = (**imageCache.currentImage).getSquareSprite().getGlobalBounds();
    auto view = window.getView();
    view.setCenter(view.getCenter().x, bounds.top + bounds.height / 2);
    window.setView(view);
    */
}

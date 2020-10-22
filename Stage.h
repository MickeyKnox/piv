#pragma once

#include <SFML/Graphics.hpp>

class Stage
{
    public:
        enum class SubType { ScrollView, SingleView };
        virtual bool instanceOf(const SubType&) = 0;

        virtual void handle(sf::Event&) = 0;
        virtual void draw(void) = 0;
        virtual void resizeEvent(sf::Event::SizeEvent&) = 0;
        virtual void scrollToPosition(float) = 0;
};

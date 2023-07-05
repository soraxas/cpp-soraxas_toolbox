/*
 * MIT License
 *
 * Copyright (c) 2019-2023 Tin Yiu Lai (@soraxas)
 *
 * This file is part of the project soraxas_toolbox, a collections of utilities
 * for developing c++ applications.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <moveit_msgs/ObjectColor.h>
#include <std_msgs/ColorRGBA.h>

namespace colour
{

    class Colour
    {
    public:
        float r;
        float g;
        float b;
        float a;

        Colour(float red, float green, float blue, float alpha = 1.f)
          : r(red), g(green), b(blue), a(alpha)
        {
        }

        // cast to rviz colour RGB msg
        operator std_msgs::ColorRGBA() const
        {
            std_msgs::ColorRGBA msg;
            msg.r = r;
            msg.g = g;
            msg.b = b;
            msg.a = a;
            return msg;
        }

        // cast to moveit object colour RGB msg
        operator moveit_msgs::ObjectColor() const
        {
            moveit_msgs::ObjectColor msg;
            msg.color = *this;
            return msg;
        }
    };

    inline Colour Colour256(int red, int green, int blue, int alpha = 255)
    {
        return Colour(red / 255.f, green / 255.f, blue / 255.f, alpha / 255.f);
    }

    inline const Colour Black = Colour256(0, 0, 0);
    inline const Colour Maroon = Colour256(128, 0, 0);
    inline const Colour Green = Colour256(0, 128, 0);
    inline const Colour Olive = Colour256(128, 128, 0);
    inline const Colour Navy = Colour256(0, 0, 128);
    inline const Colour Purple = Colour256(128, 0, 128);
    inline const Colour Teal = Colour256(0, 128, 128);
    inline const Colour Silver = Colour256(192, 192, 192);
    inline const Colour Grey = Colour256(128, 128, 128);
    inline const Colour Red = Colour256(255, 0, 0);
    inline const Colour Lime = Colour256(0, 255, 0);
    inline const Colour Yellow = Colour256(255, 255, 0);
    inline const Colour Blue = Colour256(0, 0, 255);
    inline const Colour Fuchsia = Colour256(255, 0, 255);
    inline const Colour Aqua = Colour256(0, 255, 255);
    inline const Colour White = Colour256(255, 255, 255);
    inline const Colour Grey0 = Colour256(0, 0, 0);
    inline const Colour NavyBlue = Colour256(0, 0, 95);
    inline const Colour DarkBlue = Colour256(0, 0, 135);
    inline const Colour Blue3 = Colour256(0, 0, 175);
    inline const Colour Blue4 = Colour256(0, 0, 215);
    inline const Colour Blue1 = Colour256(0, 0, 255);
    inline const Colour DarkGreen = Colour256(0, 95, 0);
    inline const Colour DeepSkyBlue4 = Colour256(0, 95, 95);
    inline const Colour DeepSkyBlue5 = Colour256(0, 95, 135);
    inline const Colour DeepSkyBlue6 = Colour256(0, 95, 175);
    inline const Colour DodgerBlue3 = Colour256(0, 95, 215);
    inline const Colour DodgerBlue2 = Colour256(0, 95, 255);
    inline const Colour Green4 = Colour256(0, 135, 0);
    inline const Colour SpringGreen4 = Colour256(0, 135, 95);
    inline const Colour Turquoise4 = Colour256(0, 135, 135);
    inline const Colour DeepSkyBlue3 = Colour256(0, 135, 175);
    inline const Colour DeepSkyBlue7 = Colour256(0, 135, 215);
    inline const Colour DodgerBlue1 = Colour256(0, 135, 255);
    inline const Colour SpringGreen3 = Colour256(0, 175, 95);
    inline const Colour DarkCyan = Colour256(0, 175, 135);
    inline const Colour LightSeaGreen = Colour256(0, 175, 175);
    inline const Colour DeepSkyBlue2 = Colour256(0, 175, 215);
    inline const Colour DeepSkyBlue1 = Colour256(0, 175, 255);
    inline const Colour Green3 = Colour256(0, 215, 0);
    inline const Colour SpringGreen2 = Colour256(0, 215, 135);
    inline const Colour Cyan3 = Colour256(0, 215, 175);
    inline const Colour DarkTurquoise = Colour256(0, 215, 215);
    inline const Colour Turquoise2 = Colour256(0, 215, 255);
    inline const Colour Green1 = Colour256(0, 255, 0);
    inline const Colour SpringGreen1 = Colour256(0, 255, 135);
    inline const Colour MediumSpringGreen = Colour256(0, 255, 175);
    inline const Colour Cyan2 = Colour256(0, 255, 215);
    inline const Colour Cyan1 = Colour256(0, 255, 255);
    inline const Colour DarkRed = Colour256(95, 0, 0);
    inline const Colour DeepPink4 = Colour256(95, 0, 95);
    inline const Colour Purple4 = Colour256(95, 0, 175);
    inline const Colour Purple3 = Colour256(95, 0, 215);
    inline const Colour BlueViolet = Colour256(95, 0, 255);
    inline const Colour Orange4 = Colour256(95, 95, 0);
    inline const Colour Grey37 = Colour256(95, 95, 95);
    inline const Colour MediumPurple4 = Colour256(95, 95, 135);
    inline const Colour SlateBlue3 = Colour256(95, 95, 175);
    inline const Colour SlateBlue4 = Colour256(95, 95, 215);
    inline const Colour RoyalBlue1 = Colour256(95, 95, 255);
    inline const Colour Chartreuse4 = Colour256(95, 135, 0);
    inline const Colour PaleTurquoise4 = Colour256(95, 135, 135);
    inline const Colour SteelBlue = Colour256(95, 135, 175);
    inline const Colour SteelBlue3 = Colour256(95, 135, 215);
    inline const Colour CornflowerBlue = Colour256(95, 135, 255);
    inline const Colour Chartreuse3 = Colour256(95, 175, 0);
    inline const Colour DarkSeaGreen4 = Colour256(95, 175, 95);
    inline const Colour CadetBlue = Colour256(95, 175, 135);
    inline const Colour SkyBlue3 = Colour256(95, 175, 215);
    inline const Colour SteelBlue1 = Colour256(95, 175, 255);
    inline const Colour PaleGreen3 = Colour256(95, 215, 95);
    inline const Colour SeaGreen3 = Colour256(95, 215, 135);
    inline const Colour Aquamarine3 = Colour256(95, 215, 175);
    inline const Colour MediumTurquoise = Colour256(95, 215, 215);
    inline const Colour Chartreuse2 = Colour256(95, 255, 0);
    inline const Colour SeaGreen2 = Colour256(95, 255, 95);
    inline const Colour SeaGreen1 = Colour256(95, 255, 175);
    inline const Colour Aquamarine1 = Colour256(95, 255, 215);
    inline const Colour DarkSlateGray2 = Colour256(95, 255, 255);
    inline const Colour DarkMagenta = Colour256(135, 0, 135);
    inline const Colour DarkViolet = Colour256(135, 0, 215);
    inline const Colour LightPink4 = Colour256(135, 95, 95);
    inline const Colour Plum4 = Colour256(135, 95, 135);
    inline const Colour MediumPurple3 = Colour256(135, 95, 175);
    inline const Colour SlateBlue1 = Colour256(135, 95, 255);
    inline const Colour Wheat4 = Colour256(135, 135, 95);
    inline const Colour Grey53 = Colour256(135, 135, 135);
    inline const Colour LightSlateGrey = Colour256(135, 135, 175);
    inline const Colour MediumPurple = Colour256(135, 135, 215);
    inline const Colour LightSlateBlue = Colour256(135, 135, 255);
    inline const Colour Yellow4 = Colour256(135, 175, 0);
    inline const Colour LightSkyBlue3 = Colour256(135, 175, 215);
    inline const Colour SkyBlue2 = Colour256(135, 175, 255);
    inline const Colour DarkOliveGreen3 = Colour256(135, 215, 95);
    inline const Colour DarkSlateGray3 = Colour256(135, 215, 215);
    inline const Colour SkyBlue1 = Colour256(135, 215, 255);
    inline const Colour Chartreuse1 = Colour256(135, 255, 0);
    inline const Colour PaleGreen1 = Colour256(135, 255, 175);
    inline const Colour DarkSlateGray1 = Colour256(135, 255, 255);
    inline const Colour MediumVioletRed = Colour256(175, 0, 135);
    inline const Colour MediumOrchid3 = Colour256(175, 95, 175);
    inline const Colour MediumOrchid = Colour256(175, 95, 215);
    inline const Colour MediumPurple2 = Colour256(175, 95, 255);
    inline const Colour DarkGoldenrod = Colour256(175, 135, 0);
    inline const Colour RosyBrown = Colour256(175, 135, 135);
    inline const Colour Grey63 = Colour256(175, 135, 175);
    inline const Colour DarkKhaki = Colour256(175, 175, 95);
    inline const Colour NavajoWhite3 = Colour256(175, 175, 135);
    inline const Colour Grey69 = Colour256(175, 175, 175);
    inline const Colour LightSteelBlue3 = Colour256(175, 175, 215);
    inline const Colour LightSteelBlue = Colour256(175, 175, 255);
    inline const Colour DarkSeaGreen3 = Colour256(175, 215, 135);
    inline const Colour DarkSeaGreen2 = Colour256(175, 215, 175);
    inline const Colour LightCyan3 = Colour256(175, 215, 215);
    inline const Colour LightSkyBlue1 = Colour256(175, 215, 255);
    inline const Colour GreenYellow = Colour256(175, 255, 0);
    inline const Colour DarkOliveGreen2 = Colour256(175, 255, 95);
    inline const Colour DarkSeaGreen1 = Colour256(175, 255, 215);
    inline const Colour PaleTurquoise1 = Colour256(175, 255, 255);
    inline const Colour Red3 = Colour256(215, 0, 0);
    inline const Colour DeepPink3 = Colour256(215, 0, 95);
    inline const Colour Magenta3 = Colour256(215, 0, 215);
    inline const Colour Magenta2 = Colour256(215, 0, 255);
    inline const Colour DarkOrange3 = Colour256(215, 95, 0);
    inline const Colour IndianRed = Colour256(215, 95, 95);
    inline const Colour HotPink3 = Colour256(215, 95, 135);
    inline const Colour HotPink2 = Colour256(215, 95, 175);
    inline const Colour Orchid = Colour256(215, 95, 215);
    inline const Colour MediumOrchid1 = Colour256(215, 95, 255);
    inline const Colour Orange3 = Colour256(215, 135, 0);
    inline const Colour LightSalmon3 = Colour256(215, 135, 95);
    inline const Colour LightPink3 = Colour256(215, 135, 135);
    inline const Colour Pink3 = Colour256(215, 135, 175);
    inline const Colour Plum3 = Colour256(215, 135, 215);
    inline const Colour Violet = Colour256(215, 135, 255);
    inline const Colour Gold3 = Colour256(215, 175, 0);
    inline const Colour LightGoldenrod3 = Colour256(215, 175, 95);
    inline const Colour Tan = Colour256(215, 175, 135);
    inline const Colour MistyRose3 = Colour256(215, 175, 175);
    inline const Colour Thistle3 = Colour256(215, 175, 215);
    inline const Colour Plum2 = Colour256(215, 175, 255);
    inline const Colour Yellow3 = Colour256(215, 215, 0);
    inline const Colour Khaki3 = Colour256(215, 215, 95);
    inline const Colour LightGoldenrod2 = Colour256(215, 215, 135);
    inline const Colour LightYellow3 = Colour256(215, 215, 175);
    inline const Colour Grey84 = Colour256(215, 215, 215);
    inline const Colour LightSteelBlue1 = Colour256(215, 215, 255);
    inline const Colour Yellow2 = Colour256(215, 255, 0);
    inline const Colour DarkOliveGreen1 = Colour256(215, 255, 95);
    inline const Colour Honeydew2 = Colour256(215, 255, 215);
    inline const Colour LightCyan1 = Colour256(215, 255, 255);
    inline const Colour Red1 = Colour256(255, 0, 0);
    inline const Colour DeepPink2 = Colour256(255, 0, 95);
    inline const Colour DeepPink1 = Colour256(255, 0, 135);
    inline const Colour Magenta1 = Colour256(255, 0, 255);
    inline const Colour OrangeRed1 = Colour256(255, 95, 0);
    inline const Colour IndianRed1 = Colour256(255, 95, 95);
    inline const Colour HotPink = Colour256(255, 95, 175);
    inline const Colour DarkOrange = Colour256(255, 135, 0);
    inline const Colour Salmon1 = Colour256(255, 135, 95);
    inline const Colour LightCoral = Colour256(255, 135, 135);
    inline const Colour PaleVioletRed1 = Colour256(255, 135, 175);
    inline const Colour Orchid2 = Colour256(255, 135, 215);
    inline const Colour Orchid1 = Colour256(255, 135, 255);
    inline const Colour Orange1 = Colour256(255, 175, 0);
    inline const Colour SandyBrown = Colour256(255, 175, 95);
    inline const Colour LightSalmon1 = Colour256(255, 175, 135);
    inline const Colour LightPink1 = Colour256(255, 175, 175);
    inline const Colour Pink1 = Colour256(255, 175, 215);
    inline const Colour Plum1 = Colour256(255, 175, 255);
    inline const Colour Gold1 = Colour256(255, 215, 0);
    inline const Colour NavajoWhite1 = Colour256(255, 215, 175);
    inline const Colour MistyRose1 = Colour256(255, 215, 215);
    inline const Colour Thistle1 = Colour256(255, 215, 255);
    inline const Colour Yellow1 = Colour256(255, 255, 0);
    inline const Colour LightGoldenrod1 = Colour256(255, 255, 95);
    inline const Colour Khaki1 = Colour256(255, 255, 135);
    inline const Colour Wheat1 = Colour256(255, 255, 175);
    inline const Colour Cornsilk1 = Colour256(255, 255, 215);
    inline const Colour Grey100 = Colour256(255, 255, 255);
    inline const Colour Grey3 = Colour256(8, 8, 8);
    inline const Colour Grey7 = Colour256(18, 18, 18);
    inline const Colour Grey11 = Colour256(28, 28, 28);
    inline const Colour Grey15 = Colour256(38, 38, 38);
    inline const Colour Grey19 = Colour256(48, 48, 48);
    inline const Colour Grey23 = Colour256(58, 58, 58);
    inline const Colour Grey27 = Colour256(68, 68, 68);
    inline const Colour Grey30 = Colour256(78, 78, 78);
    inline const Colour Grey35 = Colour256(88, 88, 88);
    inline const Colour Grey39 = Colour256(98, 98, 98);
    inline const Colour Grey42 = Colour256(108, 108, 108);
    inline const Colour Grey46 = Colour256(118, 118, 118);
    inline const Colour Grey50 = Colour256(128, 128, 128);
    inline const Colour Grey54 = Colour256(138, 138, 138);
    inline const Colour Grey58 = Colour256(148, 148, 148);
    inline const Colour Grey62 = Colour256(158, 158, 158);
    inline const Colour Grey66 = Colour256(168, 168, 168);
    inline const Colour Grey70 = Colour256(178, 178, 178);
    inline const Colour Grey74 = Colour256(188, 188, 188);
    inline const Colour Grey78 = Colour256(198, 198, 198);
    inline const Colour Grey82 = Colour256(208, 208, 208);
    inline const Colour Grey85 = Colour256(218, 218, 218);
    inline const Colour Grey89 = Colour256(228, 228, 228);
    inline const Colour Grey93 = Colour256(238, 238, 238);

}  // namespace colour

using colour::Colour;
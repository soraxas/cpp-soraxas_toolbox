#pragma once

#include <std_msgs/ColorRGBA.h>
#include <moveit_msgs/ObjectColor.h>

namespace colour
{

    class Colour
    {
    public:
        float r;
        float g;
        float b;
        float a;

        Colour(float red, float green, float blue, float alpha = 1.) : r(red), g(green), b(blue), a(alpha)
        {}
        Colour(int red, int green, int blue, int alpha = 255) : r(red/255.0), g(green/255.0), b(blue/255.0), a(alpha/255.0)
        {}

        // cast to rviz colour RGB msg
        operator std_msgs::ColorRGBA() const {
            std_msgs::ColorRGBA msg;
            msg.r = r;
            msg.g = g;
            msg.b = b;
            msg.a = a;
            return msg;
        }

        // cast to moveit object colour RGB msg
        operator moveit_msgs::ObjectColor() const {
            moveit_msgs::ObjectColor msg;
            msg.color = *this;
            return msg;
        }
    };

    const Colour Black             = Colour(0,   0,   0);
    const Colour Maroon            = Colour(128, 0,   0);
    const Colour Green             = Colour(0,   128, 0);
    const Colour Olive             = Colour(128, 128, 0);
    const Colour Navy              = Colour(0,   0,   128);
    const Colour Purple            = Colour(128, 0,   128);
    const Colour Teal              = Colour(0,   128, 128);
    const Colour Silver            = Colour(192, 192, 192);
    const Colour Grey              = Colour(128, 128, 128);
    const Colour Red               = Colour(255, 0,   0);
    const Colour Lime              = Colour(0,   255, 0);
    const Colour Yellow            = Colour(255, 255, 0);
    const Colour Blue              = Colour(0,   0,   255);
    const Colour Fuchsia           = Colour(255, 0,   255);
    const Colour Aqua              = Colour(0,   255, 255);
    const Colour White             = Colour(255, 255, 255);
    const Colour Grey0             = Colour(0,   0,   0);
    const Colour NavyBlue          = Colour(0,   0,   95);
    const Colour DarkBlue          = Colour(0,   0,   135);
    const Colour Blue3             = Colour(0,   0,   175);
    const Colour Blue4             = Colour(0,   0,   215);
    const Colour Blue1             = Colour(0,   0,   255);
    const Colour DarkGreen         = Colour(0,   95,  0);
    const Colour DeepSkyBlue4      = Colour(0,   95,  95);
    const Colour DeepSkyBlue5      = Colour(0,   95,  135);
    const Colour DeepSkyBlue6      = Colour(0,   95,  175);
    const Colour DodgerBlue3       = Colour(0,   95,  215);
    const Colour DodgerBlue2       = Colour(0,   95,  255);
    const Colour Green4            = Colour(0,   135, 0);
    const Colour SpringGreen4      = Colour(0,   135, 95);
    const Colour Turquoise4        = Colour(0,   135, 135);
    const Colour DeepSkyBlue3      = Colour(0,   135, 175);
    const Colour DeepSkyBlue7      = Colour(0,   135, 215);
    const Colour DodgerBlue1       = Colour(0,   135, 255);
    const Colour SpringGreen3      = Colour(0,   175, 95);
    const Colour DarkCyan          = Colour(0,   175, 135);
    const Colour LightSeaGreen     = Colour(0,   175, 175);
    const Colour DeepSkyBlue2      = Colour(0,   175, 215);
    const Colour DeepSkyBlue1      = Colour(0,   175, 255);
    const Colour Green3            = Colour(0,   215, 0);
    const Colour SpringGreen2      = Colour(0,   215, 135);
    const Colour Cyan3             = Colour(0,   215, 175);
    const Colour DarkTurquoise     = Colour(0,   215, 215);
    const Colour Turquoise2        = Colour(0,   215, 255);
    const Colour Green1            = Colour(0,   255, 0);
    const Colour SpringGreen1      = Colour(0,   255, 135);
    const Colour MediumSpringGreen = Colour(0,   255, 175);
    const Colour Cyan2             = Colour(0,   255, 215);
    const Colour Cyan1             = Colour(0,   255, 255);
    const Colour DarkRed           = Colour(95,  0,   0);
    const Colour DeepPink4         = Colour(95,  0,   95);
    const Colour Purple4           = Colour(95,  0,   175);
    const Colour Purple3           = Colour(95,  0,   215);
    const Colour BlueViolet        = Colour(95,  0,   255);
    const Colour Orange4           = Colour(95,  95,  0);
    const Colour Grey37            = Colour(95,  95,  95);
    const Colour MediumPurple4     = Colour(95,  95,  135);
    const Colour SlateBlue3        = Colour(95,  95,  175);
    const Colour SlateBlue4        = Colour(95,  95,  215);
    const Colour RoyalBlue1        = Colour(95,  95,  255);
    const Colour Chartreuse4       = Colour(95,  135, 0);
    const Colour PaleTurquoise4    = Colour(95,  135, 135);
    const Colour SteelBlue         = Colour(95,  135, 175);
    const Colour SteelBlue3        = Colour(95,  135, 215);
    const Colour CornflowerBlue    = Colour(95,  135, 255);
    const Colour Chartreuse3       = Colour(95,  175, 0);
    const Colour DarkSeaGreen4     = Colour(95,  175, 95);
    const Colour CadetBlue         = Colour(95,  175, 135);
    const Colour SkyBlue3          = Colour(95,  175, 215);
    const Colour SteelBlue1        = Colour(95,  175, 255);
    const Colour PaleGreen3        = Colour(95,  215, 95);
    const Colour SeaGreen3         = Colour(95,  215, 135);
    const Colour Aquamarine3       = Colour(95,  215, 175);
    const Colour MediumTurquoise   = Colour(95,  215, 215);
    const Colour Chartreuse2       = Colour(95,  255, 0);
    const Colour SeaGreen2         = Colour(95,  255, 95);
    const Colour SeaGreen1         = Colour(95,  255, 175);
    const Colour Aquamarine1       = Colour(95,  255, 215);
    const Colour DarkSlateGray2    = Colour(95,  255, 255);
    const Colour DarkMagenta       = Colour(135, 0,   135);
    const Colour DarkViolet        = Colour(135, 0,   215);
    const Colour LightPink4        = Colour(135, 95,  95);
    const Colour Plum4             = Colour(135, 95,  135);
    const Colour MediumPurple3     = Colour(135, 95,  175);
    const Colour SlateBlue1        = Colour(135, 95,  255);
    const Colour Wheat4            = Colour(135, 135, 95);
    const Colour Grey53            = Colour(135, 135, 135);
    const Colour LightSlateGrey    = Colour(135, 135, 175);
    const Colour MediumPurple      = Colour(135, 135, 215);
    const Colour LightSlateBlue    = Colour(135, 135, 255);
    const Colour Yellow4           = Colour(135, 175, 0);
    const Colour LightSkyBlue3     = Colour(135, 175, 215);
    const Colour SkyBlue2          = Colour(135, 175, 255);
    const Colour DarkOliveGreen3   = Colour(135, 215, 95);
    const Colour DarkSlateGray3    = Colour(135, 215, 215);
    const Colour SkyBlue1          = Colour(135, 215, 255);
    const Colour Chartreuse1       = Colour(135, 255, 0);
    const Colour PaleGreen1        = Colour(135, 255, 175);
    const Colour DarkSlateGray1    = Colour(135, 255, 255);
    const Colour MediumVioletRed   = Colour(175, 0,   135);
    const Colour MediumOrchid3     = Colour(175, 95,  175);
    const Colour MediumOrchid      = Colour(175, 95,  215);
    const Colour MediumPurple2     = Colour(175, 95,  255);
    const Colour DarkGoldenrod     = Colour(175, 135, 0);
    const Colour RosyBrown         = Colour(175, 135, 135);
    const Colour Grey63            = Colour(175, 135, 175);
    const Colour DarkKhaki         = Colour(175, 175, 95);
    const Colour NavajoWhite3      = Colour(175, 175, 135);
    const Colour Grey69            = Colour(175, 175, 175);
    const Colour LightSteelBlue3   = Colour(175, 175, 215);
    const Colour LightSteelBlue    = Colour(175, 175, 255);
    const Colour DarkSeaGreen3     = Colour(175, 215, 135);
    const Colour DarkSeaGreen2     = Colour(175, 215, 175);
    const Colour LightCyan3        = Colour(175, 215, 215);
    const Colour LightSkyBlue1     = Colour(175, 215, 255);
    const Colour GreenYellow       = Colour(175, 255, 0);
    const Colour DarkOliveGreen2   = Colour(175, 255, 95);
    const Colour DarkSeaGreen1     = Colour(175, 255, 215);
    const Colour PaleTurquoise1    = Colour(175, 255, 255);
    const Colour Red3              = Colour(215, 0,   0);
    const Colour DeepPink3         = Colour(215, 0,   95);
    const Colour Magenta3          = Colour(215, 0,   215);
    const Colour Magenta2          = Colour(215, 0,   255);
    const Colour DarkOrange3       = Colour(215, 95,  0);
    const Colour IndianRed         = Colour(215, 95,  95);
    const Colour HotPink3          = Colour(215, 95,  135);
    const Colour HotPink2          = Colour(215, 95,  175);
    const Colour Orchid            = Colour(215, 95,  215);
    const Colour MediumOrchid1     = Colour(215, 95,  255);
    const Colour Orange3           = Colour(215, 135, 0);
    const Colour LightSalmon3      = Colour(215, 135, 95);
    const Colour LightPink3        = Colour(215, 135, 135);
    const Colour Pink3             = Colour(215, 135, 175);
    const Colour Plum3             = Colour(215, 135, 215);
    const Colour Violet            = Colour(215, 135, 255);
    const Colour Gold3             = Colour(215, 175, 0);
    const Colour LightGoldenrod3   = Colour(215, 175, 95);
    const Colour Tan               = Colour(215, 175, 135);
    const Colour MistyRose3        = Colour(215, 175, 175);
    const Colour Thistle3          = Colour(215, 175, 215);
    const Colour Plum2             = Colour(215, 175, 255);
    const Colour Yellow3           = Colour(215, 215, 0);
    const Colour Khaki3            = Colour(215, 215, 95);
    const Colour LightGoldenrod2   = Colour(215, 215, 135);
    const Colour LightYellow3      = Colour(215, 215, 175);
    const Colour Grey84            = Colour(215, 215, 215);
    const Colour LightSteelBlue1   = Colour(215, 215, 255);
    const Colour Yellow2           = Colour(215, 255, 0);
    const Colour DarkOliveGreen1   = Colour(215, 255, 95);
    const Colour Honeydew2         = Colour(215, 255, 215);
    const Colour LightCyan1        = Colour(215, 255, 255);
    const Colour Red1              = Colour(255, 0,   0);
    const Colour DeepPink2         = Colour(255, 0,   95);
    const Colour DeepPink1         = Colour(255, 0,   135);
    const Colour Magenta1          = Colour(255, 0,   255);
    const Colour OrangeRed1        = Colour(255, 95,  0);
    const Colour IndianRed1        = Colour(255, 95,  95);
    const Colour HotPink           = Colour(255, 95,  175);
    const Colour DarkOrange        = Colour(255, 135, 0);
    const Colour Salmon1           = Colour(255, 135, 95);
    const Colour LightCoral        = Colour(255, 135, 135);
    const Colour PaleVioletRed1    = Colour(255, 135, 175);
    const Colour Orchid2           = Colour(255, 135, 215);
    const Colour Orchid1           = Colour(255, 135, 255);
    const Colour Orange1           = Colour(255, 175, 0);
    const Colour SandyBrown        = Colour(255, 175, 95);
    const Colour LightSalmon1      = Colour(255, 175, 135);
    const Colour LightPink1        = Colour(255, 175, 175);
    const Colour Pink1             = Colour(255, 175, 215);
    const Colour Plum1             = Colour(255, 175, 255);
    const Colour Gold1             = Colour(255, 215, 0);
    const Colour NavajoWhite1      = Colour(255, 215, 175);
    const Colour MistyRose1        = Colour(255, 215, 215);
    const Colour Thistle1          = Colour(255, 215, 255);
    const Colour Yellow1           = Colour(255, 255, 0);
    const Colour LightGoldenrod1   = Colour(255, 255, 95);
    const Colour Khaki1            = Colour(255, 255, 135);
    const Colour Wheat1            = Colour(255, 255, 175);
    const Colour Cornsilk1         = Colour(255, 255, 215);
    const Colour Grey100           = Colour(255, 255, 255);
    const Colour Grey3             = Colour(8,   8,   8);
    const Colour Grey7             = Colour(18,  18,  18);
    const Colour Grey11            = Colour(28,  28,  28);
    const Colour Grey15            = Colour(38,  38,  38);
    const Colour Grey19            = Colour(48,  48,  48);
    const Colour Grey23            = Colour(58,  58,  58);
    const Colour Grey27            = Colour(68,  68,  68);
    const Colour Grey30            = Colour(78,  78,  78);
    const Colour Grey35            = Colour(88,  88,  88);
    const Colour Grey39            = Colour(98,  98,  98);
    const Colour Grey42            = Colour(108, 108, 108);
    const Colour Grey46            = Colour(118, 118, 118);
    const Colour Grey50            = Colour(128, 128, 128);
    const Colour Grey54            = Colour(138, 138, 138);
    const Colour Grey58            = Colour(148, 148, 148);
    const Colour Grey62            = Colour(158, 158, 158);
    const Colour Grey66            = Colour(168, 168, 168);
    const Colour Grey70            = Colour(178, 178, 178);
    const Colour Grey74            = Colour(188, 188, 188);
    const Colour Grey78            = Colour(198, 198, 198);
    const Colour Grey82            = Colour(208, 208, 208);
    const Colour Grey85            = Colour(218, 218, 218);
    const Colour Grey89            = Colour(228, 228, 228);
    const Colour Grey93            = Colour(238, 238, 238);

} // namespace colour

using colour::Colour;
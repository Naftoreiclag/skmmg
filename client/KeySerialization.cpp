#include "KeySerialization.hpp"

namespace skm
{

sf::Keyboard::Key KeySerialization::toKey(std::string str) {
    if(str == "Space") { return sf::Keyboard::Key::Space; }
    else if(str == "A") { return sf::Keyboard::Key::A; }
    else if(str == "B") { return sf::Keyboard::Key::B; }
    else if(str == "C") { return sf::Keyboard::Key::C; }
    else if(str == "D") { return sf::Keyboard::Key::D; }
    else if(str == "E") { return sf::Keyboard::Key::E; }
    else if(str == "F") { return sf::Keyboard::Key::F; }
    else if(str == "G") { return sf::Keyboard::Key::G; }
    else if(str == "H") { return sf::Keyboard::Key::H; }
    else if(str == "I") { return sf::Keyboard::Key::I; }
    else if(str == "J") { return sf::Keyboard::Key::J; }
    else if(str == "K") { return sf::Keyboard::Key::K; }
    else if(str == "L") { return sf::Keyboard::Key::L; }
    else if(str == "M") { return sf::Keyboard::Key::M; }
    else if(str == "N") { return sf::Keyboard::Key::N; }
    else if(str == "O") { return sf::Keyboard::Key::O; }
    else if(str == "P") { return sf::Keyboard::Key::P; }
    else if(str == "Q") { return sf::Keyboard::Key::Q; }
    else if(str == "R") { return sf::Keyboard::Key::R; }
    else if(str == "S") { return sf::Keyboard::Key::S; }
    else if(str == "T") { return sf::Keyboard::Key::T; }
    else if(str == "U") { return sf::Keyboard::Key::U; }
    else if(str == "V") { return sf::Keyboard::Key::V; }
    else if(str == "W") { return sf::Keyboard::Key::W; }
    else if(str == "X") { return sf::Keyboard::Key::X; }
    else if(str == "Y") { return sf::Keyboard::Key::Y; }
    else if(str == "Z") { return sf::Keyboard::Key::Z; }
    else if(str == "Num0") { return sf::Keyboard::Key::Num0; }
    else if(str == "Num1") { return sf::Keyboard::Key::Num1; }
    else if(str == "Num2") { return sf::Keyboard::Key::Num2; }
    else if(str == "Num3") { return sf::Keyboard::Key::Num3; }
    else if(str == "Num4") { return sf::Keyboard::Key::Num4; }
    else if(str == "Num5") { return sf::Keyboard::Key::Num5; }
    else if(str == "Num6") { return sf::Keyboard::Key::Num6; }
    else if(str == "Num7") { return sf::Keyboard::Key::Num7; }
    else if(str == "Num8") { return sf::Keyboard::Key::Num8; }
    else if(str == "Num9") { return sf::Keyboard::Key::Num9; }
    else if(str == "Escape") { return sf::Keyboard::Key::Escape; }
    else if(str == "LControl") { return sf::Keyboard::Key::LControl; }
    else if(str == "LShift") { return sf::Keyboard::Key::LShift; }
    else if(str == "LAlt") { return sf::Keyboard::Key::LAlt; }
    else if(str == "LSystem") { return sf::Keyboard::Key::LSystem; }
    else if(str == "RControl") { return sf::Keyboard::Key::RControl; }
    else if(str == "RShift") { return sf::Keyboard::Key::RShift; }
    else if(str == "RAlt") { return sf::Keyboard::Key::RAlt; }
    else if(str == "RSystem") { return sf::Keyboard::Key::RSystem; }
    else if(str == "Menu") { return sf::Keyboard::Key::Menu; }
    else if(str == "LBracket") { return sf::Keyboard::Key::LBracket; }
    else if(str == "RBracket") { return sf::Keyboard::Key::RBracket; }
    else if(str == "SemiColon") { return sf::Keyboard::Key::SemiColon; }
    else if(str == "Comma") { return sf::Keyboard::Key::Comma; }
    else if(str == "Period") { return sf::Keyboard::Key::Period; }
    else if(str == "Quote") { return sf::Keyboard::Key::Quote; }
    else if(str == "Slash") { return sf::Keyboard::Key::Slash; }
    else if(str == "BackSlash") { return sf::Keyboard::Key::BackSlash; }
    else if(str == "Tilde") { return sf::Keyboard::Key::Tilde; }
    else if(str == "Equal") { return sf::Keyboard::Key::Equal; }
    else if(str == "Dash") { return sf::Keyboard::Key::Dash; }
    else if(str == "Return") { return sf::Keyboard::Key::Return; }
    else if(str == "BackSpace") { return sf::Keyboard::Key::BackSpace; }
    else if(str == "Tab") { return sf::Keyboard::Key::Tab; }
    else if(str == "PageUp") { return sf::Keyboard::Key::PageUp; }
    else if(str == "PageDown") { return sf::Keyboard::Key::PageDown; }
    else if(str == "End") { return sf::Keyboard::Key::End; }
    else if(str == "Home") { return sf::Keyboard::Key::Home; }
    else if(str == "Insert") { return sf::Keyboard::Key::Insert; }
    else if(str == "Delete") { return sf::Keyboard::Key::Delete; }
    else if(str == "Add") { return sf::Keyboard::Key::Add; }
    else if(str == "Subtract") { return sf::Keyboard::Key::Subtract; }
    else if(str == "Multiply") { return sf::Keyboard::Key::Multiply; }
    else if(str == "Divide") { return sf::Keyboard::Key::Divide; }
    else if(str == "Left") { return sf::Keyboard::Key::Left; }
    else if(str == "Right") { return sf::Keyboard::Key::Right; }
    else if(str == "Up") { return sf::Keyboard::Key::Up; }
    else if(str == "Down") { return sf::Keyboard::Key::Down; }
    else if(str == "Numpad0") { return sf::Keyboard::Key::Numpad0; }
    else if(str == "Numpad1") { return sf::Keyboard::Key::Numpad1; }
    else if(str == "Numpad2") { return sf::Keyboard::Key::Numpad2; }
    else if(str == "Numpad3") { return sf::Keyboard::Key::Numpad3; }
    else if(str == "Numpad4") { return sf::Keyboard::Key::Numpad4; }
    else if(str == "Numpad5") { return sf::Keyboard::Key::Numpad5; }
    else if(str == "Numpad6") { return sf::Keyboard::Key::Numpad6; }
    else if(str == "Numpad7") { return sf::Keyboard::Key::Numpad7; }
    else if(str == "Numpad8") { return sf::Keyboard::Key::Numpad8; }
    else if(str == "Numpad9") { return sf::Keyboard::Key::Numpad9; }
    else if(str == "F1") { return sf::Keyboard::Key::F1; }
    else if(str == "F2") { return sf::Keyboard::Key::F2; }
    else if(str == "F3") { return sf::Keyboard::Key::F3; }
    else if(str == "F4") { return sf::Keyboard::Key::F4; }
    else if(str == "F5") { return sf::Keyboard::Key::F5; }
    else if(str == "F6") { return sf::Keyboard::Key::F6; }
    else if(str == "F7") { return sf::Keyboard::Key::F7; }
    else if(str == "F8") { return sf::Keyboard::Key::F8; }
    else if(str == "F9") { return sf::Keyboard::Key::F9; }
    else if(str == "F10") { return sf::Keyboard::Key::F10; }
    else if(str == "F11") { return sf::Keyboard::Key::F11; }
    else if(str == "F12") { return sf::Keyboard::Key::F12; }
    else if(str == "F13") { return sf::Keyboard::Key::F13; }
    else if(str == "F14") { return sf::Keyboard::Key::F14; }
    else if(str == "F15") { return sf::Keyboard::Key::F15; }
    else if(str == "Pause") { return sf::Keyboard::Key::Pause; }
    else { return sf::Keyboard::Key::Unknown; }
}

std::string KeySerialization::toString(sf::Keyboard::Key key) {
    switch(key) {
        case sf::Keyboard::Key::Space: { return "Space"; }
        case sf::Keyboard::Key::A: { return "A"; }
        case sf::Keyboard::Key::B: { return "B"; }
        case sf::Keyboard::Key::C: { return "C"; }
        case sf::Keyboard::Key::D: { return "D"; }
        case sf::Keyboard::Key::E: { return "E"; }
        case sf::Keyboard::Key::F: { return "F"; }
        case sf::Keyboard::Key::G: { return "G"; }
        case sf::Keyboard::Key::H: { return "H"; }
        case sf::Keyboard::Key::I: { return "I"; }
        case sf::Keyboard::Key::J: { return "J"; }
        case sf::Keyboard::Key::K: { return "K"; }
        case sf::Keyboard::Key::L: { return "L"; }
        case sf::Keyboard::Key::M: { return "M"; }
        case sf::Keyboard::Key::N: { return "N"; }
        case sf::Keyboard::Key::O: { return "O"; }
        case sf::Keyboard::Key::P: { return "P"; }
        case sf::Keyboard::Key::Q: { return "Q"; }
        case sf::Keyboard::Key::R: { return "R"; }
        case sf::Keyboard::Key::S: { return "S"; }
        case sf::Keyboard::Key::T: { return "T"; }
        case sf::Keyboard::Key::U: { return "U"; }
        case sf::Keyboard::Key::V: { return "V"; }
        case sf::Keyboard::Key::W: { return "W"; }
        case sf::Keyboard::Key::X: { return "X"; }
        case sf::Keyboard::Key::Y: { return "Y"; }
        case sf::Keyboard::Key::Z: { return "Z"; }
        case sf::Keyboard::Key::Num0: { return "Num0"; }
        case sf::Keyboard::Key::Num1: { return "Num1"; }
        case sf::Keyboard::Key::Num2: { return "Num2"; }
        case sf::Keyboard::Key::Num3: { return "Num3"; }
        case sf::Keyboard::Key::Num4: { return "Num4"; }
        case sf::Keyboard::Key::Num5: { return "Num5"; }
        case sf::Keyboard::Key::Num6: { return "Num6"; }
        case sf::Keyboard::Key::Num7: { return "Num7"; }
        case sf::Keyboard::Key::Num8: { return "Num8"; }
        case sf::Keyboard::Key::Num9: { return "Num9"; }
        case sf::Keyboard::Key::Escape: { return "Escape"; }
        case sf::Keyboard::Key::LControl: { return "LControl"; }
        case sf::Keyboard::Key::LShift: { return "LShift"; }
        case sf::Keyboard::Key::LAlt: { return "LAlt"; }
        case sf::Keyboard::Key::LSystem: { return "LSystem"; }
        case sf::Keyboard::Key::RControl: { return "RControl"; }
        case sf::Keyboard::Key::RShift: { return "RShift"; }
        case sf::Keyboard::Key::RAlt: { return "RAlt"; }
        case sf::Keyboard::Key::RSystem: { return "RSystem"; }
        case sf::Keyboard::Key::Menu: { return "Menu"; }
        case sf::Keyboard::Key::LBracket: { return "LBracket"; }
        case sf::Keyboard::Key::RBracket: { return "RBracket"; }
        case sf::Keyboard::Key::SemiColon: { return "SemiColon"; }
        case sf::Keyboard::Key::Comma: { return "Comma"; }
        case sf::Keyboard::Key::Period: { return "Period"; }
        case sf::Keyboard::Key::Quote: { return "Quote"; }
        case sf::Keyboard::Key::Slash: { return "Slash"; }
        case sf::Keyboard::Key::BackSlash: { return "BackSlash"; }
        case sf::Keyboard::Key::Tilde: { return "Tilde"; }
        case sf::Keyboard::Key::Equal: { return "Equal"; }
        case sf::Keyboard::Key::Dash: { return "Dash"; }
        case sf::Keyboard::Key::Return: { return "Return"; }
        case sf::Keyboard::Key::BackSpace: { return "BackSpace"; }
        case sf::Keyboard::Key::Tab: { return "Tab"; }
        case sf::Keyboard::Key::PageUp: { return "PageUp"; }
        case sf::Keyboard::Key::PageDown: { return "PageDown"; }
        case sf::Keyboard::Key::End: { return "End"; }
        case sf::Keyboard::Key::Home: { return "Home"; }
        case sf::Keyboard::Key::Insert: { return "Insert"; }
        case sf::Keyboard::Key::Delete: { return "Delete"; }
        case sf::Keyboard::Key::Add: { return "Add"; }
        case sf::Keyboard::Key::Subtract: { return "Subtract"; }
        case sf::Keyboard::Key::Multiply: { return "Multiply"; }
        case sf::Keyboard::Key::Divide: { return "Divide"; }
        case sf::Keyboard::Key::Left: { return "Left"; }
        case sf::Keyboard::Key::Right: { return "Right"; }
        case sf::Keyboard::Key::Up: { return "Up"; }
        case sf::Keyboard::Key::Down: { return "Down"; }
        case sf::Keyboard::Key::Numpad0: { return "Numpad0"; }
        case sf::Keyboard::Key::Numpad1: { return "Numpad1"; }
        case sf::Keyboard::Key::Numpad2: { return "Numpad2"; }
        case sf::Keyboard::Key::Numpad3: { return "Numpad3"; }
        case sf::Keyboard::Key::Numpad4: { return "Numpad4"; }
        case sf::Keyboard::Key::Numpad5: { return "Numpad5"; }
        case sf::Keyboard::Key::Numpad6: { return "Numpad6"; }
        case sf::Keyboard::Key::Numpad7: { return "Numpad7"; }
        case sf::Keyboard::Key::Numpad8: { return "Numpad8"; }
        case sf::Keyboard::Key::Numpad9: { return "Numpad9"; }
        case sf::Keyboard::Key::F1: { return "F1"; }
        case sf::Keyboard::Key::F2: { return "F2"; }
        case sf::Keyboard::Key::F3: { return "F3"; }
        case sf::Keyboard::Key::F4: { return "F4"; }
        case sf::Keyboard::Key::F5: { return "F5"; }
        case sf::Keyboard::Key::F6: { return "F6"; }
        case sf::Keyboard::Key::F7: { return "F7"; }
        case sf::Keyboard::Key::F8: { return "F8"; }
        case sf::Keyboard::Key::F9: { return "F9"; }
        case sf::Keyboard::Key::F10: { return "F10"; }
        case sf::Keyboard::Key::F11: { return "F11"; }
        case sf::Keyboard::Key::F12: { return "F12"; }
        case sf::Keyboard::Key::F13: { return "F13"; }
        case sf::Keyboard::Key::F14: { return "F14"; }
        case sf::Keyboard::Key::F15: { return "F15"; }
        case sf::Keyboard::Key::Pause: { return "Pause"; }
        default: { return "Unset"; }
    }
}

}


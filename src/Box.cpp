/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "Box.h"

DBU Box::getHPWL() {
        DBU lowerX = _lowerBound.getX();
        DBU lowerY = _lowerBound.getY();
        DBU upperX = _upperBound.getX();
        DBU upperY = _upperBound.getY();

        DBU x = upperX - lowerX;
        DBU y = upperY - lowerY;

        return (x + y);
}
#include "lsystem.h"
#include <iostream>

LSystem::LSystem()
    :axiom(QString("FFFXY")), turtle(Turtle()), turtleHistory(), savedStates()
{
    expansionOperations.insert('X', QString("C[AFFBFFX]BFFBFFX"));
    expansionOperations.insert('Y', QString("C[AFFFFFY]BFAFFAFFFAFFF"));
    turtleHistory.push(turtle);
//    Rule = &Rule1;
//    void (*LSystem::Rule)(void) = &drawLine;
//    drawingOperations.insert('X', LSystem::drawLine());
}


LSystem::LSystem(QString axiom, int x_boundary_start, int x_boundary_end, int z_boundary_start, int z_boundary_end)
    :axiom(axiom), turtle(Turtle(glm::vec2(x_boundary_start, z_boundary_start), glm::vec2(0.0, 1.0), 1)), turtleHistory(), savedStates()
{
    expansionOperations.insert('X', QString("C[AFFFBFFX]BFFBFFX"));
    expansionOperations.insert('Y', QString("C[AFFFFFY]BFAFFAFFFAFFF"));
    turtleHistory.push(turtle);
}


QString LSystem::expandGrammar(const QString& s) const {
    QString new_s;
    int count = 0;
    while (count < s.length()) {
        if(expansionOperations.contains(s.at(count))) {
            new_s.append(expansionOperations.value(s.at(count)));
        }
        else {
            new_s.append(s.at(count));
        }
        count = count + 1;
    }
    return new_s;
}

void LSystem::executeRule(const QChar s, const int index) {
    if(s == QChar('F')) {
        drawLine();
    }
    if(s == QChar('A')) {
        rotateTurtleCW();
    }
    if(s == QChar('B')) {
        rotateTurtleCCW();
    }
    if(s == 'C') {
        //Use some probability function to determine whether to branch or not
        if(rand() % 100 < 90) {
            //If we decide to branch, save the turtle state until a close bracket is encountered
            savedStates.push_back(turtleHistory.last());
        }
        else {
            //If we decide not to branch, remove the branching grammar from the axiom
            removeGrammar(index);
        }
    }
    //If close bracket encountered pop off the last saved turtle state and proceed from there
    if(s == ']') {
        Turtle saved = savedStates.last();
        savedStates.pop_back();
        turtleHistory.push_back(saved);
        this->turtle = saved;
    }
}

void LSystem::removeGrammar(const int index) {
    int count = 0;
    int end_index;
    for(int i = index + 1; i < axiom.length(); i++) {
        if(axiom.at(i) == '[') {
            count = count + 1;
        }
        if(axiom.at(i) == ']') {
            count = count - 1;
            if(count == 0) {
                end_index = i;
            }
        }
    }
    std::string new_axiom = axiom.toStdString().substr(0, index) + axiom.toStdString().substr(end_index);
    axiom = QString::fromStdString(new_axiom);
}

void LSystem::drawLine() {
    turtle.pos.x += 30 * turtle.orient.x;
    turtle.pos.y += 30 * turtle.orient.y;
    turtle.depth += 1;
    turtleHistory.push_back(turtle);
}

void LSystem::rotateTurtleCW() {
    //Positive x
    if(turtle.orient.x == 1.0)
    {
        turtle.orient.x = 0.5;
        turtle.orient.y = 0.5;
    }
    //Negative x
    else if(turtle.orient.x == -1.0)
    {
        turtle.orient.x = -0.5;
        turtle.orient.y = -0.5;
    }
    //Positive z
    else if(turtle.orient.x == 0.0 && turtle.orient.y == 1.0)
    {
        turtle.orient.x = -0.5;
        turtle.orient.y = 0.5;
    }
    //Negative z
    else if(turtle.orient.x == 0.0 && turtle.orient.y == -1.0)
    {
        turtle.orient.x = 0.5;
        turtle.orient.y = -0.5;
    }
    //Positive x and z
    else if(turtle.orient.x == 0.5 && turtle.orient.y == 0.5)
    {
        turtle.orient.x = 0.0;
        turtle.orient.y = 1.0;
    }
    //Positive x negative z
    else if(turtle.orient.x == 0.5 && turtle.orient.y == -0.5)
    {
        turtle.orient.x = 1.0;
        turtle.orient.y = 0.0;
    }
    //Negative x and z
    else if(turtle.orient.x == -0.5 && turtle.orient.y == -0.5)
    {
        turtle.orient.x = 0.0;
        turtle.orient.y = -1.0;
    }
    //Negative x positive z
    else if(turtle.orient.x == -0.5 && turtle.orient.y == 0.5)
    {
        turtle.orient.x = -1.0;
        turtle.orient.y = 0.0;
    }
    turtleHistory.push_back(turtle);
}

void LSystem::rotateTurtleCCW() {
    //Positive x
    if(turtle.orient.x == 1.0)
    {
        turtle.orient.x = 0.5;
        turtle.orient.y = -0.5;
    }
    //Negative x
    else if(turtle.orient.x == -1.0)
    {
        turtle.orient.x = -0.5;
        turtle.orient.y = 0.5;
    }
    //Positive z
    else if(turtle.orient.x == 0.0 && turtle.orient.y == 1.0)
    {
        turtle.orient.x = 0.5;
        turtle.orient.y = 0.5;
    }
    //Negative z
    else if(turtle.orient.x == 0.0 && turtle.orient.y == -1.0)
    {
        turtle.orient.x = -0.5;
        turtle.orient.y = -0.5;
    }
    //Positive x and z
    else if(turtle.orient.x == 0.5 && turtle.orient.y == 0.5)
    {
        turtle.orient.x = 1.0;
        turtle.orient.y = 0.0;
    }
    //Positive x negative z
    else if(turtle.orient.x == 0.5 && turtle.orient.y == -0.5)
    {
        turtle.orient.x = 0.0;
        turtle.orient.y = -1.0;
    }
    //Negative x and z
    else if(turtle.orient.x == -0.5 && turtle.orient.y == -0.5)
    {
        turtle.orient.x = -1.0;
        turtle.orient.y = 0.0;
    }
    //Negative x positive z
    else if(turtle.orient.x == -0.5 && turtle.orient.y == 0.5)
    {
        turtle.orient.x = 0.0;
        turtle.orient.y = 1.0;
    }
    turtleHistory.push_back(turtle);
}



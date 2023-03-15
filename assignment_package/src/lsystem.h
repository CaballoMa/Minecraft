#pragma once

#include "glm_includes.h"
#include <QList>
#include <QStack>
#include <QHash>
#include <QString>
#include <QChar>

struct Turtle {
    glm::vec2 pos;
    glm::vec2 orient;
    int depth;

    Turtle(glm::vec2 p, glm::vec2 o, int d)
        : pos(p), orient(o), depth(d)
    {}

    Turtle(const Turtle& t)
        : pos(t.pos), orient(t.orient), depth(t.depth)
    {}

    Turtle()
        : pos(0, 0), orient(1.0, 0.0), depth(1)
    {}
};

typedef void (*Rule)(void);

class LSystem {
public:
    QString axiom;
    QHash<QChar, QString> expansionOperations;
    QHash<QChar, Rule> drawingOperations;
    Turtle turtle;
    QStack<Turtle> turtleHistory;
    QStack<Turtle> savedStates;

    LSystem();

    LSystem(QString axiom, int xStart, int xEnd, int zStart, int zEnd);

    QString expandGrammar(const QString& s) const; // Iterate over each character in the axiom
                                                   // and replace it with its expansion if applicable

    void drawLine(); // Move turtle forward and draw a line from its start to end
    void rotateTurtleCW(); // Rotate turtle 45 degrees CW
    void rotateTurtleCCW(); // Rotate turtle 45 degrees CCW
    void saveTurtleState(); // Push the turtle's current state onto turtleHistory
    void popTurtleState(); // Pop the turtleHistory stack and set turtle's members to that Turtle's members

    void executeRule(const QChar s, const int index);

    void removeGrammar(const int index);
};


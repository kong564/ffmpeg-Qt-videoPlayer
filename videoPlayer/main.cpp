#include "controller.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    videoPlayer player;
    Controller ctl(&player);
    player.show();

    return a.exec();
}

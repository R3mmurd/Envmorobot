/*
  Envmorobot.

  Author: Alejandro Mujica (aledrums@gmail.com)
*/

# include <QApplication>
# include <QTextCodec>
# include <mapframe.H>

int main(int argc, char * argv[])
{
  QApplication app(argc, argv);

  QTextCodec * textCodec = QTextCodec::codecForName("UTF-8");
  QTextCodec::setCodecForLocale(textCodec);

  QLocale::setDefault(QLocale(QLocale::Spanish, QLocale::Venezuela));

  MapFrame frame;
  frame.show();

  return app.exec();
}

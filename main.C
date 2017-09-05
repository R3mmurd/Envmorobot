/*
  This file is part of Ropab System.
  Copyright (C) 2012 by Alejandro J. Mujica

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Any user request of this software, write to 

  Alejandro Mujica

  aledrums@gmail.com
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

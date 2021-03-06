/**
 * This file is part of eelsmodel.
 *
 * eelsmodel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * eelsmodel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with eelsmodel.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Jo Verbeeck, Ruben Van Boxem
 * Copyright: 2002-2013 Jo Verbeeck
 *
 **/

/**
 * eelsmodel - gui/monitorchooser.cpp
 **/

#include "src/gui/monitorchooser.h"

#include <QInputDialog>
#include <QString>
#include <QStringList>

MonitorChooser::MonitorChooser(QWidget *parent, const char *name,int& retvalue ) : QWidget(parent) {
QStringList list;
QString s1="difference p1-p2";
list.append( s1 );
QString s2= "sum p1+p2";
list.append( s2 );
QString s3= "ratio p1/p2";
list.append( s3 );
bool ok=false;
//QString result=QInputDialog::getItem ("Monitor Chooser","Choose a monitor type to use",list,0,false,&ok,0,"monitor chooser");
QString result=QInputDialog::getItem (this,"Monitor Chooser","Choose a monitor type to use",list,0,false,&ok);

  if ( ok ){
    if (result==s1){
      retvalue=0;
      }
    if (result==s2){
      retvalue=1;
      }
    if (result==s3){
      retvalue=2;
      }
    if ((result!=s3)&&(result!=s2)&&(result!=s1)){
      retvalue=-1;
      }
}
  else
      retvalue=-1;// user pressed Cancel
}
MonitorChooser::~MonitorChooser(){
}

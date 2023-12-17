import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import {SensorsComponent} from "./components/sensors/sensors.component";
import {DataViewComponent} from "./components/data-view/data-view.component";
import {AddRoomDialogComponent} from "./components/add-room-dialog/add-room-dialog.component";
import {AddRoomDialoggComponent} from "./components/add-room-dialogg/add-room-dialogg.component";
import {AppComponent} from "./components/app/app.component";

const routes: Routes = [
  {
    path: '',
    component:AppComponent,
    children:[
      {path:"sensors" ,component: SensorsComponent,pathMatch:'full'},
      {path:"dataView" ,component:DataViewComponent,pathMatch:'full'},
      {path:"rooms" ,component:AddRoomDialogComponent,pathMatch :'full'},
      {path:"rooms1" ,component:AddRoomDialoggComponent,pathMatch :'full'}
    ]
  }
];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule]
})
export class DashboardRoutingModule { }

import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import {DataViewComponent} from "./shared/components/data-view/data-view.component";
import {SensorsComponent} from "./shared/components/sensors/sensors.component";
import {AddRoomDialogComponent} from "./shared/components/add-room-dialog/add-room-dialog.component";

const routes: Routes = [
  { path: 'dataView', component: DataViewComponent, pathMatch: 'full' },
  { path: 'sensors', component: SensorsComponent, pathMatch: 'full' },
  {path: 'rooms', component: AddRoomDialogComponent, pathMatch: 'full'},
  { path: '', redirectTo: '/dataView', pathMatch: 'full' },
];


@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }

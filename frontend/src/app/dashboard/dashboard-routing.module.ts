import {NgModule} from '@angular/core';
import {RouterModule, Routes} from '@angular/router';
import {HomeComponent} from './components/home/home.component';
import {RoomComponent} from "./components/room/room.component";
import {AddRoomComponent} from "./components/add-room/add-room.component";
import {UpdateRoomComponent} from "./components/update-room/update-room.component";
import {GrundrissComponent} from './components/grundriss/grundriss.component';
import {PlanDialogComponent} from "./components/plan-dialog/plan-dialog.component";

const routes: Routes = [
  {path: 'home', component: HomeComponent, pathMatch: 'full'},
  {path: 'rooms', component: AddRoomComponent, pathMatch: 'full'},
  {path: 'rooms/edit/:id', component: UpdateRoomComponent, pathMatch: 'full'},
  {path: 'rooms/:id', component: RoomComponent, pathMatch: 'full'},
  {path: 'grundriss', component: GrundrissComponent, pathMatch: 'full'},
  {path: 'grundriss/create', component: PlanDialogComponent, pathMatch: 'full'},
  {path: '', redirectTo: 'home', pathMatch: 'full'},
];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule]
})
export class DashboardRoutingModule {
}

import {NgModule} from '@angular/core';
import {RouterModule, Routes} from '@angular/router';
import {HomeComponent} from './components/home/home.component';
import {RoomComponent} from "./components/room/room.component";
import {AddRoomComponent} from "./components/add-room/add-room.component";

const routes: Routes = [
  {path: 'home', component: HomeComponent, pathMatch: 'full'},
  {path: 'rooms', component: AddRoomComponent, pathMatch: 'full'},
  {path: 'rooms/:id', component: RoomComponent, pathMatch: 'full'},
  {path: '', redirectTo: 'home', pathMatch: 'full'},
];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule]
})
export class DashboardRoutingModule {
}

import {NgModule} from '@angular/core';
import {RouterModule, Routes} from '@angular/router';
import {HomeComponent} from './components/home/home.component';
import {RoomComponent} from "./components/room/room.component";
import {AddRoomComponent} from "./components/add-room/add-room.component";
import {UpdateRoomComponent} from "./components/update-room/update-room.component";
import {PlansComponent} from './components/plans/plans.component';
import {PlanComponent} from "./components/plan/plan.component";

const routes: Routes = [
  {path: 'home', component: HomeComponent, pathMatch: 'full'},
  {path: 'rooms', component: AddRoomComponent, pathMatch: 'full'},
  {path: 'rooms/edit/:id', component: UpdateRoomComponent, pathMatch: 'full'},
  {path: 'rooms/:id', component: RoomComponent, pathMatch: 'full'},
  {path: 'plan', component: PlansComponent, pathMatch: 'full'},
  {path: 'plan/create', component: PlanComponent, pathMatch: 'full'},
  {path: '', redirectTo: 'home', pathMatch: 'full'},
];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule]
})
export class DashboardRoutingModule {
}

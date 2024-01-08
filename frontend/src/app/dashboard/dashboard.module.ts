import {NgModule} from '@angular/core';
import {MatCardModule} from "@angular/material/card";
import {ReactiveFormsModule} from "@angular/forms";
import {MatDialogModule} from "@angular/material/dialog";
import {MatInputModule} from "@angular/material/input";
import {DashboardRoutingModule} from "./dashboard-routing.module";
import {MatButtonModule} from "@angular/material/button";
import {MatSelectModule} from "@angular/material/select";
import {MatOptionModule} from '@angular/material/core';
import {HomeComponent} from './components/home/home.component';
import {SharedModule} from "../shared/shared.module";
import {NgForOf, NgIf} from "@angular/common";
import {RoomComponent} from './components/room/room.component';


@NgModule({
  declarations: [
    HomeComponent,
    RoomComponent,
  ],
  exports: [],
  imports: [
    DashboardRoutingModule,
    MatCardModule,
    ReactiveFormsModule,
    MatDialogModule,
    MatInputModule,
    MatButtonModule,
    MatSelectModule,
    MatOptionModule,
    SharedModule,
    NgForOf,
    NgIf,
  ]
})
export class DashboardModule {
}

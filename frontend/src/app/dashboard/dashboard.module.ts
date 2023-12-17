import { NgModule } from '@angular/core';
import {SensorsComponent} from "./components/sensors/sensors.component";
import {MatCardModule} from "@angular/material/card";
import {ReactiveFormsModule} from "@angular/forms";
import {MatDialogModule} from "@angular/material/dialog";
import {MatInputModule} from "@angular/material/input";
import {DashboardRoutingModule} from "./dashboard-routing.module";
import { AppComponent } from './components/app/app.component';
import { DataViewComponent } from './components/data-view/data-view.component';
import { AddRoomDialogComponent } from './components/add-room-dialog/add-room-dialog.component';
import {MatButtonModule} from "@angular/material/button";
import {MatSelectModule} from "@angular/material/select";
import { MatOptionModule } from '@angular/material/core';
import { AddRoomDialoggComponent } from './components/add-room-dialogg/add-room-dialogg.component';



@NgModule({
  declarations: [
AddRoomDialogComponent,
    SensorsComponent,
    AppComponent,
    DataViewComponent,
    AddRoomDialoggComponent,



  ],
  exports: [
    AddRoomDialogComponent,
    SensorsComponent,
    DataViewComponent
  ],
  imports: [
    DashboardRoutingModule,
    MatCardModule,
    ReactiveFormsModule,
    MatDialogModule,
    MatInputModule,
    MatButtonModule,
    MatSelectModule,
    MatOptionModule,

  ]
})
export class DashboardModule { }

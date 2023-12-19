import { NgModule } from '@angular/core';
import {SensorsComponent} from "./components/sensors/sensors.component";
import {MatCardModule} from "@angular/material/card";
import {FormsModule, ReactiveFormsModule} from "@angular/forms";
import {MatDialogModule} from "@angular/material/dialog";
import {MatInputModule} from "@angular/material/input";
import {DashboardRoutingModule} from "./dashboard-routing.module";
import { AppComponent } from './components/app/app.component';
import { DataViewComponent } from './components/data-view/data-view.component';
import {MatButtonModule} from "@angular/material/button";
import {MatSelectModule} from "@angular/material/select";
import { NgMultiSelectDropDownModule } from 'ng-multiselect-dropdown';
import { AddRoomDialoggComponent } from './components/add-room-dialogg/add-room-dialogg.component';
import {NgForOf, NgIf} from "@angular/common";



@NgModule({
  declarations: [
    SensorsComponent,
    AppComponent,
    DataViewComponent,
    AddRoomDialoggComponent,

  ],
  exports: [
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
    NgMultiSelectDropDownModule.forRoot(),
    MatSelectModule,
    FormsModule,
    NgIf,
    NgForOf,

  ]
})
export class DashboardModule { }

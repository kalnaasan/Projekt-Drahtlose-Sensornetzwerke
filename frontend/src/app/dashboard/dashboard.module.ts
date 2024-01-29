import {NgModule} from '@angular/core';
import {SensorsComponent} from "./components/sensors/sensors.component";
import {MatCardModule} from "@angular/material/card";
import {FormsModule, ReactiveFormsModule} from "@angular/forms";
import {MatDialogModule} from "@angular/material/dialog";
import {MatInputModule} from "@angular/material/input";
import {DashboardRoutingModule} from "./dashboard-routing.module";
import {AppComponent} from './components/app/app.component';
import {DataViewComponent} from './components/data-view/data-view.component';
import {MatButtonModule} from "@angular/material/button";
import {MatSelectModule} from "@angular/material/select";
import {MatOptionModule} from '@angular/material/core';
import {NgMultiSelectDropDownModule} from 'ng-multiselect-dropdown';
import {AddRoomComponent} from './components/add-room/add-room.component';
import {HomeComponent} from './components/home/home.component';
import {SharedModule} from "../shared/shared.module";
import {NgForOf, NgIf, NgSwitch, NgSwitchCase, NgSwitchDefault} from "@angular/common";
import {RoomComponent} from './components/room/room.component';
import {UpdateRoomComponent} from './components/update-room/update-room.component';
import {GrundrissComponent} from './components/grundriss/grundriss.component';
import {StatusDialogComponent} from './components/status-dialog/status-dialog.component';
import {PlanDialogComponent} from './components/plan-dialog/plan-dialog.component';
import {MatIconModule} from "@angular/material/icon";


@NgModule({
  declarations: [
    HomeComponent,
    RoomComponent,
    SensorsComponent,
    AppComponent,
    DataViewComponent,
    AddRoomComponent,
    UpdateRoomComponent,
    GrundrissComponent,
    StatusDialogComponent,
    PlanDialogComponent
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
    MatOptionModule,
    SharedModule,
    NgForOf,
    NgIf,
    FormsModule,
    NgIf,
    NgForOf,
    NgSwitch,
    NgSwitchCase,
    NgSwitchDefault,
    MatIconModule,
  ]
})
export class DashboardModule {
}

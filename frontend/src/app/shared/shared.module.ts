import {NgModule} from '@angular/core';
import {CommonModule} from '@angular/common';
import {NavbarComponent} from './components/navbar/navbar.component';
import {MatIconModule} from "@angular/material/icon";
import {MatToolbarModule} from "@angular/material/toolbar";
import {MatButtonModule} from "@angular/material/button";
import {MatSidenavModule} from "@angular/material/sidenav";
import {DataViewComponent} from './components/data-view/data-view.component';
import {MatCardModule} from "@angular/material/card";
import {SidebarComponent} from "./components/sidebar/sidebar.component";
import {SensorsComponent} from './components/sensors/sensors.component';
import {RouterModule} from '@angular/router';
import {AddRoomDialogComponent} from './components/add-room-dialog/add-room-dialog.component';
import {MatDialogModule} from "@angular/material/dialog";
import {ReactiveFormsModule} from "@angular/forms";
import {MatInputModule} from "@angular/material/input";
import {NgApexchartsModule} from 'ng-apexcharts';
import {ChartComponent} from './components/chart/chart.component';


@NgModule({
  declarations: [
    NavbarComponent,
    SidebarComponent,
    DataViewComponent,
    SensorsComponent,
    AddRoomDialogComponent,
    ChartComponent,
  ],
    exports: [
        NavbarComponent,
        SidebarComponent,
        DataViewComponent,
        SensorsComponent,
        AddRoomDialogComponent,
        ChartComponent
    ],
  imports: [
    CommonModule,
    MatIconModule,
    MatToolbarModule,
    MatButtonModule,
    MatSidenavModule,
    MatCardModule,
    RouterModule,
    MatDialogModule,
    ReactiveFormsModule,
    MatInputModule,
    NgApexchartsModule,
  ]
})
export class SharedModule {
}

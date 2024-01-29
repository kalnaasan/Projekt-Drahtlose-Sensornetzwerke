import {NgModule} from '@angular/core';
import {BrowserModule} from '@angular/platform-browser';

import {AppRoutingModule} from './app-routing.module';
import {AppComponent} from './app.component';
import {BrowserAnimationsModule} from '@angular/platform-browser/animations';
import {SharedModule} from "./shared/shared.module";
import {DashboardModule} from "./dashboard/dashboard.module";
import {HttpClientModule} from "@angular/common/http";
import {DatePipe} from "@angular/common";

@NgModule({
  declarations: [
    AppComponent
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,
    BrowserAnimationsModule,
    HttpClientModule,
    DashboardModule,
    SharedModule
  ],
  providers: [DatePipe],
  bootstrap: [AppComponent]
})
export class AppModule {
}

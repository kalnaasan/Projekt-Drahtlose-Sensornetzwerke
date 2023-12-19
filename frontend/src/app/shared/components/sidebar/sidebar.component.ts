// sidebar.component.ts
import { Component, Input, OnInit } from '@angular/core';
import { RouteItem } from "../../common/route-item";
import { Router } from "@angular/router";

@Component({
  selector: 'app-sidebar',
  templateUrl: './sidebar.component.html',
  styleUrls: ['./sidebar.component.scss']
})
export class SidebarComponent implements OnInit {

  @Input()
  public sideBarOpen = true;
  public routeItems: RouteItem[] = new Array<RouteItem>();

  constructor(private router: Router) {
  }

  ngOnInit(): void {
    // You can populate routeItems here if needed
  }

  goToSensor() {
    // Use the Router to navigate to the 'shared/sensors' route
    this.router.navigate(['dashboard/sensors']);
  }

  addNewRaum() {
    this.router.navigate(['dashboard/rooms1']);
  }
}

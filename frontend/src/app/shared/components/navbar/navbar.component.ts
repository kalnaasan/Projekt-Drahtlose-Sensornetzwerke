import {Component, EventEmitter, Input, Output} from '@angular/core';
import {Router} from "@angular/router";

@Component({
  selector: 'app-navbar',
  templateUrl: './navbar.component.html',
  styleUrls: ['./navbar.component.scss']
})
export class NavbarComponent {
  constructor(private router: Router) {
  }

  @Output() sideBarToogle = new EventEmitter();

  @Input() title = '';
  longText: any;

  toogleSideBar() {
    this.sideBarToogle.emit();
  }
}

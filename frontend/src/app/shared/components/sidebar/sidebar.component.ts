import {Component, Input, OnInit} from '@angular/core';
import {RouteItem} from "../../common/route-item";
import {Router} from "@angular/router";
import {RoomService} from "../../services/RoomService";

@Component({
  selector: 'app-sidebar',
  templateUrl: './sidebar.component.html',
  styleUrls: ['./sidebar.component.scss']
})
export class SidebarComponent implements OnInit {

  @Input()
  public sideBarOpen = true;
  public routeItems: RouteItem[] = new Array<RouteItem>();

  constructor(private router: Router,
              private roomService: RoomService) {
  }

  ngOnInit(): void {
    this.roomService.getAllRooms().subscribe({
      next: (res: any) => {
        for (const item of res.data) {
          const routeItem: RouteItem = {
            name: item.name,
            link: '/rooms/' + item.id,
            icon: 'room'
          }
          this.routeItems.push(routeItem);
        }
      },
      error: (err: any) => console.log(err)
    });
  }

  addNewRaum() {
    this.router.navigate(['/rooms']).then(() => {
    });
  }
}

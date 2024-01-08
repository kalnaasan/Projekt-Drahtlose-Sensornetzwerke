import {Component, Input, OnInit} from '@angular/core';
import {RouteItem} from "../../common/route-item";
import {Router} from "@angular/router";
import {HttpClient} from "@angular/common/http";

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
              private http: HttpClient) {
  }

  ngOnInit(): void {
    this.http.get<any>('http://localhost:8080/api/v1/web/rooms').subscribe({
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

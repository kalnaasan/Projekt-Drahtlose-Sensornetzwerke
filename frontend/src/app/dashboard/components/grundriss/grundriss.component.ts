import {Component, ElementRef, OnInit, ViewChild} from '@angular/core';
import {ShapeService} from "../../../shared/services/shape.service";
import {MatDialog} from "@angular/material/dialog";
import {PlanDialogComponent} from "../plan-dialog/plan-dialog.component";

@Component({
  selector: 'app-grundriss',
  templateUrl: './grundriss.component.html',
  styleUrls: ['./grundriss.component.scss']
})
export class GrundrissComponent implements OnInit {
  @ViewChild('svgContainer', {static: true}) svgContainer!: ElementRef;

  constructor(private shapeService: ShapeService,
              private dialog: MatDialog) {
  }

  ngOnInit(): void {
    this.shapeService.getShapes().subscribe({
      next: (res: any) => console.log(res),
      error: (err: any) => console.log(err)
    });
  }

  openDialog() {
    const dialogRef = this.dialog.open(PlanDialogComponent, {});
  }
}

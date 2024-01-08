import { ComponentFixture, TestBed } from '@angular/core/testing';

import { AddRoomDialoggComponent } from './add-room-dialogg.component';

describe('AddRoomDialoggComponent', () => {
  let component: AddRoomDialoggComponent;
  let fixture: ComponentFixture<AddRoomDialoggComponent>;

  beforeEach(() => {
    TestBed.configureTestingModule({
      declarations: [AddRoomDialoggComponent]
    });
    fixture = TestBed.createComponent(AddRoomDialoggComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
